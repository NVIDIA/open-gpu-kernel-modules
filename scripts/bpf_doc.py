#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2018-2019 Netronome Systems, Inc.
# Copyright (C) 2021 Isovalent, Inc.

# In case user attempts to run with Python 2.
from __future__ import print_function

import argparse
import re
import sys, os

class NoHelperFound(BaseException):
    pass

class NoSyscallCommandFound(BaseException):
    pass

class ParsingError(BaseException):
    def __init__(self, line='<line not provided>', reader=None):
        if reader:
            BaseException.__init__(self,
                                   'Error at file offset %d, parsing line: %s' %
                                   (reader.tell(), line))
        else:
            BaseException.__init__(self, 'Error parsing line: %s' % line)


class APIElement(object):
    """
    An object representing the description of an aspect of the eBPF API.
    @proto: prototype of the API symbol
    @desc: textual description of the symbol
    @ret: (optional) description of any associated return value
    """
    def __init__(self, proto='', desc='', ret=''):
        self.proto = proto
        self.desc = desc
        self.ret = ret


class Helper(APIElement):
    """
    An object representing the description of an eBPF helper function.
    @proto: function prototype of the helper function
    @desc: textual description of the helper function
    @ret: description of the return value of the helper function
    """
    def proto_break_down(self):
        """
        Break down helper function protocol into smaller chunks: return type,
        name, distincts arguments.
        """
        arg_re = re.compile('((\w+ )*?(\w+|...))( (\**)(\w+))?$')
        res = {}
        proto_re = re.compile('(.+) (\**)(\w+)\(((([^,]+)(, )?){1,5})\)$')

        capture = proto_re.match(self.proto)
        res['ret_type'] = capture.group(1)
        res['ret_star'] = capture.group(2)
        res['name']     = capture.group(3)
        res['args'] = []

        args    = capture.group(4).split(', ')
        for a in args:
            capture = arg_re.match(a)
            res['args'].append({
                'type' : capture.group(1),
                'star' : capture.group(5),
                'name' : capture.group(6)
            })

        return res


class HeaderParser(object):
    """
    An object used to parse a file in order to extract the documentation of a
    list of eBPF helper functions. All the helpers that can be retrieved are
    stored as Helper object, in the self.helpers() array.
    @filename: name of file to parse, usually include/uapi/linux/bpf.h in the
               kernel tree
    """
    def __init__(self, filename):
        self.reader = open(filename, 'r')
        self.line = ''
        self.helpers = []
        self.commands = []

    def parse_element(self):
        proto    = self.parse_symbol()
        desc     = self.parse_desc()
        ret      = self.parse_ret()
        return APIElement(proto=proto, desc=desc, ret=ret)

    def parse_helper(self):
        proto    = self.parse_proto()
        desc     = self.parse_desc()
        ret      = self.parse_ret()
        return Helper(proto=proto, desc=desc, ret=ret)

    def parse_symbol(self):
        p = re.compile(' \* ?(.+)$')
        capture = p.match(self.line)
        if not capture:
            raise NoSyscallCommandFound
        end_re = re.compile(' \* ?NOTES$')
        end = end_re.match(self.line)
        if end:
            raise NoSyscallCommandFound
        self.line = self.reader.readline()
        return capture.group(1)

    def parse_proto(self):
        # Argument can be of shape:
        #   - "void"
        #   - "type  name"
        #   - "type *name"
        #   - Same as above, with "const" and/or "struct" in front of type
        #   - "..." (undefined number of arguments, for bpf_trace_printk())
        # There is at least one term ("void"), and at most five arguments.
        p = re.compile(' \* ?((.+) \**\w+\((((const )?(struct )?(\w+|\.\.\.)( \**\w+)?)(, )?){1,5}\))$')
        capture = p.match(self.line)
        if not capture:
            raise NoHelperFound
        self.line = self.reader.readline()
        return capture.group(1)

    def parse_desc(self):
        p = re.compile(' \* ?(?:\t| {5,8})Description$')
        capture = p.match(self.line)
        if not capture:
            # Helper can have empty description and we might be parsing another
            # attribute: return but do not consume.
            return ''
        # Description can be several lines, some of them possibly empty, and it
        # stops when another subsection title is met.
        desc = ''
        while True:
            self.line = self.reader.readline()
            if self.line == ' *\n':
                desc += '\n'
            else:
                p = re.compile(' \* ?(?:\t| {5,8})(?:\t| {8})(.*)')
                capture = p.match(self.line)
                if capture:
                    desc += capture.group(1) + '\n'
                else:
                    break
        return desc

    def parse_ret(self):
        p = re.compile(' \* ?(?:\t| {5,8})Return$')
        capture = p.match(self.line)
        if not capture:
            # Helper can have empty retval and we might be parsing another
            # attribute: return but do not consume.
            return ''
        # Return value description can be several lines, some of them possibly
        # empty, and it stops when another subsection title is met.
        ret = ''
        while True:
            self.line = self.reader.readline()
            if self.line == ' *\n':
                ret += '\n'
            else:
                p = re.compile(' \* ?(?:\t| {5,8})(?:\t| {8})(.*)')
                capture = p.match(self.line)
                if capture:
                    ret += capture.group(1) + '\n'
                else:
                    break
        return ret

    def seek_to(self, target, help_message):
        self.reader.seek(0)
        offset = self.reader.read().find(target)
        if offset == -1:
            raise Exception(help_message)
        self.reader.seek(offset)
        self.reader.readline()
        self.reader.readline()
        self.line = self.reader.readline()

    def parse_syscall(self):
        self.seek_to('* DOC: eBPF Syscall Commands',
                     'Could not find start of eBPF syscall descriptions list')
        while True:
            try:
                command = self.parse_element()
                self.commands.append(command)
            except NoSyscallCommandFound:
                break

    def parse_helpers(self):
        self.seek_to('* Start of BPF helper function descriptions:',
                     'Could not find start of eBPF helper descriptions list')
        while True:
            try:
                helper = self.parse_helper()
                self.helpers.append(helper)
            except NoHelperFound:
                break

    def run(self):
        self.parse_syscall()
        self.parse_helpers()
        self.reader.close()

###############################################################################

class Printer(object):
    """
    A generic class for printers. Printers should be created with an array of
    Helper objects, and implement a way to print them in the desired fashion.
    @parser: A HeaderParser with objects to print to standard output
    """
    def __init__(self, parser):
        self.parser = parser
        self.elements = []

    def print_header(self):
        pass

    def print_footer(self):
        pass

    def print_one(self, helper):
        pass

    def print_all(self):
        self.print_header()
        for elem in self.elements:
            self.print_one(elem)
        self.print_footer()


class PrinterRST(Printer):
    """
    A generic class for printers that print ReStructured Text. Printers should
    be created with a HeaderParser object, and implement a way to print API
    elements in the desired fashion.
    @parser: A HeaderParser with objects to print to standard output
    """
    def __init__(self, parser):
        self.parser = parser

    def print_license(self):
        license = '''\
.. Copyright (C) All BPF authors and contributors from 2014 to present.
.. See git log include/uapi/linux/bpf.h in kernel tree for details.
.. 
.. %%%LICENSE_START(VERBATIM)
.. Permission is granted to make and distribute verbatim copies of this
.. manual provided the copyright notice and this permission notice are
.. preserved on all copies.
.. 
.. Permission is granted to copy and distribute modified versions of this
.. manual under the conditions for verbatim copying, provided that the
.. entire resulting derived work is distributed under the terms of a
.. permission notice identical to this one.
.. 
.. Since the Linux kernel and libraries are constantly changing, this
.. manual page may be incorrect or out-of-date.  The author(s) assume no
.. responsibility for errors or omissions, or for damages resulting from
.. the use of the information contained herein.  The author(s) may not
.. have taken the same level of care in the production of this manual,
.. which is licensed free of charge, as they might when working
.. professionally.
.. 
.. Formatted or processed versions of this manual, if unaccompanied by
.. the source, must acknowledge the copyright and authors of this work.
.. %%%LICENSE_END
.. 
.. Please do not edit this file. It was generated from the documentation
.. located in file include/uapi/linux/bpf.h of the Linux kernel sources
.. (helpers description), and from scripts/bpf_doc.py in the same
.. repository (header and footer).
'''
        print(license)

    def print_elem(self, elem):
        if (elem.desc):
            print('\tDescription')
            # Do not strip all newline characters: formatted code at the end of
            # a section must be followed by a blank line.
            for line in re.sub('\n$', '', elem.desc, count=1).split('\n'):
                print('{}{}'.format('\t\t' if line else '', line))

        if (elem.ret):
            print('\tReturn')
            for line in elem.ret.rstrip().split('\n'):
                print('{}{}'.format('\t\t' if line else '', line))

        print('')


class PrinterHelpersRST(PrinterRST):
    """
    A printer for dumping collected information about helpers as a ReStructured
    Text page compatible with the rst2man program, which can be used to
    generate a manual page for the helpers.
    @parser: A HeaderParser with Helper objects to print to standard output
    """
    def __init__(self, parser):
        self.elements = parser.helpers

    def print_header(self):
        header = '''\
===========
BPF-HELPERS
===========
-------------------------------------------------------------------------------
list of eBPF helper functions
-------------------------------------------------------------------------------

:Manual section: 7

DESCRIPTION
===========

The extended Berkeley Packet Filter (eBPF) subsystem consists in programs
written in a pseudo-assembly language, then attached to one of the several
kernel hooks and run in reaction of specific events. This framework differs
from the older, "classic" BPF (or "cBPF") in several aspects, one of them being
the ability to call special functions (or "helpers") from within a program.
These functions are restricted to a white-list of helpers defined in the
kernel.

These helpers are used by eBPF programs to interact with the system, or with
the context in which they work. For instance, they can be used to print
debugging messages, to get the time since the system was booted, to interact
with eBPF maps, or to manipulate network packets. Since there are several eBPF
program types, and that they do not run in the same context, each program type
can only call a subset of those helpers.

Due to eBPF conventions, a helper can not have more than five arguments.

Internally, eBPF programs call directly into the compiled helper functions
without requiring any foreign-function interface. As a result, calling helpers
introduces no overhead, thus offering excellent performance.

This document is an attempt to list and document the helpers available to eBPF
developers. They are sorted by chronological order (the oldest helpers in the
kernel at the top).

HELPERS
=======
'''
        PrinterRST.print_license(self)
        print(header)

    def print_footer(self):
        footer = '''
EXAMPLES
========

Example usage for most of the eBPF helpers listed in this manual page are
available within the Linux kernel sources, at the following locations:

* *samples/bpf/*
* *tools/testing/selftests/bpf/*

LICENSE
=======

eBPF programs can have an associated license, passed along with the bytecode
instructions to the kernel when the programs are loaded. The format for that
string is identical to the one in use for kernel modules (Dual licenses, such
as "Dual BSD/GPL", may be used). Some helper functions are only accessible to
programs that are compatible with the GNU Privacy License (GPL).

In order to use such helpers, the eBPF program must be loaded with the correct
license string passed (via **attr**) to the **bpf**\ () system call, and this
generally translates into the C source code of the program containing a line
similar to the following:

::

	char ____license[] __attribute__((section("license"), used)) = "GPL";

IMPLEMENTATION
==============

This manual page is an effort to document the existing eBPF helper functions.
But as of this writing, the BPF sub-system is under heavy development. New eBPF
program or map types are added, along with new helper functions. Some helpers
are occasionally made available for additional program types. So in spite of
the efforts of the community, this page might not be up-to-date. If you want to
check by yourself what helper functions exist in your kernel, or what types of
programs they can support, here are some files among the kernel tree that you
may be interested in:

* *include/uapi/linux/bpf.h* is the main BPF header. It contains the full list
  of all helper functions, as well as many other BPF definitions including most
  of the flags, structs or constants used by the helpers.
* *net/core/filter.c* contains the definition of most network-related helper
  functions, and the list of program types from which they can be used.
* *kernel/trace/bpf_trace.c* is the equivalent for most tracing program-related
  helpers.
* *kernel/bpf/verifier.c* contains the functions used to check that valid types
  of eBPF maps are used with a given helper function.
* *kernel/bpf/* directory contains other files in which additional helpers are
  defined (for cgroups, sockmaps, etc.).
* The bpftool utility can be used to probe the availability of helper functions
  on the system (as well as supported program and map types, and a number of
  other parameters). To do so, run **bpftool feature probe** (see
  **bpftool-feature**\ (8) for details). Add the **unprivileged** keyword to
  list features available to unprivileged users.

Compatibility between helper functions and program types can generally be found
in the files where helper functions are defined. Look for the **struct
bpf_func_proto** objects and for functions returning them: these functions
contain a list of helpers that a given program type can call. Note that the
**default:** label of the **switch ... case** used to filter helpers can call
other functions, themselves allowing access to additional helpers. The
requirement for GPL license is also in those **struct bpf_func_proto**.

Compatibility between helper functions and map types can be found in the
**check_map_func_compatibility**\ () function in file *kernel/bpf/verifier.c*.

Helper functions that invalidate the checks on **data** and **data_end**
pointers for network processing are listed in function
**bpf_helper_changes_pkt_data**\ () in file *net/core/filter.c*.

SEE ALSO
========

**bpf**\ (2),
**bpftool**\ (8),
**cgroups**\ (7),
**ip**\ (8),
**perf_event_open**\ (2),
**sendmsg**\ (2),
**socket**\ (7),
**tc-bpf**\ (8)'''
        print(footer)

    def print_proto(self, helper):
        """
        Format function protocol with bold and italics markers. This makes RST
        file less readable, but gives nice results in the manual page.
        """
        proto = helper.proto_break_down()

        print('**%s %s%s(' % (proto['ret_type'],
                              proto['ret_star'].replace('*', '\\*'),
                              proto['name']),
              end='')

        comma = ''
        for a in proto['args']:
            one_arg = '{}{}'.format(comma, a['type'])
            if a['name']:
                if a['star']:
                    one_arg += ' {}**\ '.format(a['star'].replace('*', '\\*'))
                else:
                    one_arg += '** '
                one_arg += '*{}*\\ **'.format(a['name'])
            comma = ', '
            print(one_arg, end='')

        print(')**')

    def print_one(self, helper):
        self.print_proto(helper)
        self.print_elem(helper)


class PrinterSyscallRST(PrinterRST):
    """
    A printer for dumping collected information about the syscall API as a
    ReStructured Text page compatible with the rst2man program, which can be
    used to generate a manual page for the syscall.
    @parser: A HeaderParser with APIElement objects to print to standard
             output
    """
    def __init__(self, parser):
        self.elements = parser.commands

    def print_header(self):
        header = '''\
===
bpf
===
-------------------------------------------------------------------------------
Perform a command on an extended BPF object
-------------------------------------------------------------------------------

:Manual section: 2

COMMANDS
========
'''
        PrinterRST.print_license(self)
        print(header)

    def print_one(self, command):
        print('**%s**' % (command.proto))
        self.print_elem(command)


class PrinterHelpers(Printer):
    """
    A printer for dumping collected information about helpers as C header to
    be included from BPF program.
    @parser: A HeaderParser with Helper objects to print to standard output
    """
    def __init__(self, parser):
        self.elements = parser.helpers

    type_fwds = [
            'struct bpf_fib_lookup',
            'struct bpf_sk_lookup',
            'struct bpf_perf_event_data',
            'struct bpf_perf_event_value',
            'struct bpf_pidns_info',
            'struct bpf_redir_neigh',
            'struct bpf_sock',
            'struct bpf_sock_addr',
            'struct bpf_sock_ops',
            'struct bpf_sock_tuple',
            'struct bpf_spin_lock',
            'struct bpf_sysctl',
            'struct bpf_tcp_sock',
            'struct bpf_tunnel_key',
            'struct bpf_xfrm_state',
            'struct linux_binprm',
            'struct pt_regs',
            'struct sk_reuseport_md',
            'struct sockaddr',
            'struct tcphdr',
            'struct seq_file',
            'struct tcp6_sock',
            'struct tcp_sock',
            'struct tcp_timewait_sock',
            'struct tcp_request_sock',
            'struct udp6_sock',
            'struct task_struct',

            'struct __sk_buff',
            'struct sk_msg_md',
            'struct xdp_md',
            'struct path',
            'struct btf_ptr',
            'struct inode',
            'struct socket',
            'struct file',
    ]
    known_types = {
            '...',
            'void',
            'const void',
            'char',
            'const char',
            'int',
            'long',
            'unsigned long',

            '__be16',
            '__be32',
            '__wsum',

            'struct bpf_fib_lookup',
            'struct bpf_perf_event_data',
            'struct bpf_perf_event_value',
            'struct bpf_pidns_info',
            'struct bpf_redir_neigh',
            'struct bpf_sk_lookup',
            'struct bpf_sock',
            'struct bpf_sock_addr',
            'struct bpf_sock_ops',
            'struct bpf_sock_tuple',
            'struct bpf_spin_lock',
            'struct bpf_sysctl',
            'struct bpf_tcp_sock',
            'struct bpf_tunnel_key',
            'struct bpf_xfrm_state',
            'struct linux_binprm',
            'struct pt_regs',
            'struct sk_reuseport_md',
            'struct sockaddr',
            'struct tcphdr',
            'struct seq_file',
            'struct tcp6_sock',
            'struct tcp_sock',
            'struct tcp_timewait_sock',
            'struct tcp_request_sock',
            'struct udp6_sock',
            'struct task_struct',
            'struct path',
            'struct btf_ptr',
            'struct inode',
            'struct socket',
            'struct file',
    }
    mapped_types = {
            'u8': '__u8',
            'u16': '__u16',
            'u32': '__u32',
            'u64': '__u64',
            's8': '__s8',
            's16': '__s16',
            's32': '__s32',
            's64': '__s64',
            'size_t': 'unsigned long',
            'struct bpf_map': 'void',
            'struct sk_buff': 'struct __sk_buff',
            'const struct sk_buff': 'const struct __sk_buff',
            'struct sk_msg_buff': 'struct sk_msg_md',
            'struct xdp_buff': 'struct xdp_md',
    }
    # Helpers overloaded for different context types.
    overloaded_helpers = [
        'bpf_get_socket_cookie',
        'bpf_sk_assign',
    ]

    def print_header(self):
        header = '''\
/* This is auto-generated file. See bpf_doc.py for details. */

/* Forward declarations of BPF structs */'''

        print(header)
        for fwd in self.type_fwds:
            print('%s;' % fwd)
        print('')

    def print_footer(self):
        footer = ''
        print(footer)

    def map_type(self, t):
        if t in self.known_types:
            return t
        if t in self.mapped_types:
            return self.mapped_types[t]
        print("Unrecognized type '%s', please add it to known types!" % t,
              file=sys.stderr)
        sys.exit(1)

    seen_helpers = set()

    def print_one(self, helper):
        proto = helper.proto_break_down()

        if proto['name'] in self.seen_helpers:
            return
        self.seen_helpers.add(proto['name'])

        print('/*')
        print(" * %s" % proto['name'])
        print(" *")
        if (helper.desc):
            # Do not strip all newline characters: formatted code at the end of
            # a section must be followed by a blank line.
            for line in re.sub('\n$', '', helper.desc, count=1).split('\n'):
                print(' *{}{}'.format(' \t' if line else '', line))

        if (helper.ret):
            print(' *')
            print(' * Returns')
            for line in helper.ret.rstrip().split('\n'):
                print(' *{}{}'.format(' \t' if line else '', line))

        print(' */')
        print('static %s %s(*%s)(' % (self.map_type(proto['ret_type']),
                                      proto['ret_star'], proto['name']), end='')
        comma = ''
        for i, a in enumerate(proto['args']):
            t = a['type']
            n = a['name']
            if proto['name'] in self.overloaded_helpers and i == 0:
                    t = 'void'
                    n = 'ctx'
            one_arg = '{}{}'.format(comma, self.map_type(t))
            if n:
                if a['star']:
                    one_arg += ' {}'.format(a['star'])
                else:
                    one_arg += ' '
                one_arg += '{}'.format(n)
            comma = ', '
            print(one_arg, end='')

        print(') = (void *) %d;' % len(self.seen_helpers))
        print('')

###############################################################################

# If script is launched from scripts/ from kernel tree and can access
# ../include/uapi/linux/bpf.h, use it as a default name for the file to parse,
# otherwise the --filename argument will be required from the command line.
script = os.path.abspath(sys.argv[0])
linuxRoot = os.path.dirname(os.path.dirname(script))
bpfh = os.path.join(linuxRoot, 'include/uapi/linux/bpf.h')

printers = {
        'helpers': PrinterHelpersRST,
        'syscall': PrinterSyscallRST,
}

argParser = argparse.ArgumentParser(description="""
Parse eBPF header file and generate documentation for the eBPF API.
The RST-formatted output produced can be turned into a manual page with the
rst2man utility.
""")
argParser.add_argument('--header', action='store_true',
                       help='generate C header file')
if (os.path.isfile(bpfh)):
    argParser.add_argument('--filename', help='path to include/uapi/linux/bpf.h',
                           default=bpfh)
else:
    argParser.add_argument('--filename', help='path to include/uapi/linux/bpf.h')
argParser.add_argument('target', nargs='?', default='helpers',
                       choices=printers.keys(), help='eBPF API target')
args = argParser.parse_args()

# Parse file.
headerParser = HeaderParser(args.filename)
headerParser.run()

# Print formatted output to standard output.
if args.header:
    if args.target != 'helpers':
        raise NotImplementedError('Only helpers header generation is supported')
    printer = PrinterHelpers(headerParser)
else:
    printer = printers[args.target](headerParser)
printer.print_all()
