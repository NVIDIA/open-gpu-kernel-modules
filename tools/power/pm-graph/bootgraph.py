#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
#
# Tool for analyzing boot timing
# Copyright (c) 2013, Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# Authors:
#	 Todd Brandt <todd.e.brandt@linux.intel.com>
#
# Description:
#	 This tool is designed to assist kernel and OS developers in optimizing
#	 their linux stack's boot time. It creates an html representation of
#	 the kernel boot timeline up to the start of the init process.
#

# ----------------- LIBRARIES --------------------

import sys
import time
import os
import string
import re
import platform
import shutil
from datetime import datetime, timedelta
from subprocess import call, Popen, PIPE
import sleepgraph as aslib

def pprint(msg):
	print(msg)
	sys.stdout.flush()

# ----------------- CLASSES --------------------

# Class: SystemValues
# Description:
#	 A global, single-instance container used to
#	 store system values and test parameters
class SystemValues(aslib.SystemValues):
	title = 'BootGraph'
	version = '2.2'
	hostname = 'localhost'
	testtime = ''
	kernel = ''
	dmesgfile = ''
	ftracefile = ''
	htmlfile = 'bootgraph.html'
	testdir = ''
	kparams = ''
	result = ''
	useftrace = False
	usecallgraph = False
	suspendmode = 'boot'
	max_graph_depth = 2
	graph_filter = 'do_one_initcall'
	reboot = False
	manual = False
	iscronjob = False
	timeformat = '%.6f'
	bootloader = 'grub'
	blexec = []
	def __init__(self):
		self.hostname = platform.node()
		self.testtime = datetime.now().strftime('%Y-%m-%d_%H:%M:%S')
		if os.path.exists('/proc/version'):
			fp = open('/proc/version', 'r')
			val = fp.read().strip()
			fp.close()
			self.kernel = self.kernelVersion(val)
		else:
			self.kernel = 'unknown'
		self.testdir = datetime.now().strftime('boot-%y%m%d-%H%M%S')
	def kernelVersion(self, msg):
		return msg.split()[2]
	def checkFtraceKernelVersion(self):
		val = tuple(map(int, self.kernel.split('-')[0].split('.')))
		if val >= (4, 10, 0):
			return True
		return False
	def kernelParams(self):
		cmdline = 'initcall_debug log_buf_len=32M'
		if self.useftrace:
			if self.cpucount > 0:
				bs = min(self.memtotal // 2, 2*1024*1024) // self.cpucount
			else:
				bs = 131072
			cmdline += ' trace_buf_size=%dK trace_clock=global '\
			'trace_options=nooverwrite,funcgraph-abstime,funcgraph-cpu,'\
			'funcgraph-duration,funcgraph-proc,funcgraph-tail,'\
			'nofuncgraph-overhead,context-info,graph-time '\
			'ftrace=function_graph '\
			'ftrace_graph_max_depth=%d '\
			'ftrace_graph_filter=%s' % \
				(bs, self.max_graph_depth, self.graph_filter)
		return cmdline
	def setGraphFilter(self, val):
		master = self.getBootFtraceFilterFunctions()
		fs = ''
		for i in val.split(','):
			func = i.strip()
			if func == '':
				doError('badly formatted filter function string')
			if '[' in func or ']' in func:
				doError('loadable module functions not allowed - "%s"' % func)
			if ' ' in func:
				doError('spaces found in filter functions - "%s"' % func)
			if func not in master:
				doError('function "%s" not available for ftrace' % func)
			if not fs:
				fs = func
			else:
				fs += ','+func
		if not fs:
			doError('badly formatted filter function string')
		self.graph_filter = fs
	def getBootFtraceFilterFunctions(self):
		self.rootCheck(True)
		fp = open(self.tpath+'available_filter_functions')
		fulllist = fp.read().split('\n')
		fp.close()
		list = []
		for i in fulllist:
			if not i or ' ' in i or '[' in i or ']' in i:
				continue
			list.append(i)
		return list
	def myCronJob(self, line):
		if '@reboot' not in line:
			return False
		if 'bootgraph' in line or 'analyze_boot.py' in line or '-cronjob' in line:
			return True
		return False
	def cronjobCmdString(self):
		cmdline = '%s -cronjob' % os.path.abspath(sys.argv[0])
		args = iter(sys.argv[1:])
		for arg in args:
			if arg in ['-h', '-v', '-cronjob', '-reboot', '-verbose']:
				continue
			elif arg in ['-o', '-dmesg', '-ftrace', '-func']:
				next(args)
				continue
			elif arg == '-result':
				cmdline += ' %s "%s"' % (arg, os.path.abspath(next(args)))
				continue
			elif arg == '-cgskip':
				file = self.configFile(next(args))
				cmdline += ' %s "%s"' % (arg, os.path.abspath(file))
				continue
			cmdline += ' '+arg
		if self.graph_filter != 'do_one_initcall':
			cmdline += ' -func "%s"' % self.graph_filter
		cmdline += ' -o "%s"' % os.path.abspath(self.testdir)
		return cmdline
	def manualRebootRequired(self):
		cmdline = self.kernelParams()
		pprint('To generate a new timeline manually, follow these steps:\n\n'\
		'1. Add the CMDLINE string to your kernel command line.\n'\
		'2. Reboot the system.\n'\
		'3. After reboot, re-run this tool with the same arguments but no command (w/o -reboot or -manual).\n\n'\
		'CMDLINE="%s"' % cmdline)
		sys.exit()
	def blGrub(self):
		blcmd = ''
		for cmd in ['update-grub', 'grub-mkconfig', 'grub2-mkconfig']:
			if blcmd:
				break
			blcmd = self.getExec(cmd)
		if not blcmd:
			doError('[GRUB] missing update command')
		if not os.path.exists('/etc/default/grub'):
			doError('[GRUB] missing /etc/default/grub')
		if 'grub2' in blcmd:
			cfg = '/boot/grub2/grub.cfg'
		else:
			cfg = '/boot/grub/grub.cfg'
		if not os.path.exists(cfg):
			doError('[GRUB] missing %s' % cfg)
		if 'update-grub' in blcmd:
			self.blexec = [blcmd]
		else:
			self.blexec = [blcmd, '-o', cfg]
	def getBootLoader(self):
		if self.bootloader == 'grub':
			self.blGrub()
		else:
			doError('unknown boot loader: %s' % self.bootloader)
	def writeDatafileHeader(self, filename):
		self.kparams = open('/proc/cmdline', 'r').read().strip()
		fp = open(filename, 'w')
		fp.write(self.teststamp+'\n')
		fp.write(self.sysstamp+'\n')
		fp.write('# command | %s\n' % self.cmdline)
		fp.write('# kparams | %s\n' % self.kparams)
		fp.close()

sysvals = SystemValues()

# Class: Data
# Description:
#	 The primary container for test data.
class Data(aslib.Data):
	dmesg = {}  # root data structure
	start = 0.0 # test start
	end = 0.0   # test end
	dmesgtext = []   # dmesg text file in memory
	testnumber = 0
	idstr = ''
	html_device_id = 0
	valid = False
	tUserMode = 0.0
	boottime = ''
	phases = ['kernel', 'user']
	do_one_initcall = False
	def __init__(self, num):
		self.testnumber = num
		self.idstr = 'a'
		self.dmesgtext = []
		self.dmesg = {
			'kernel': {'list': dict(), 'start': -1.0, 'end': -1.0, 'row': 0,
				'order': 0, 'color': 'linear-gradient(to bottom, #fff, #bcf)'},
			'user': {'list': dict(), 'start': -1.0, 'end': -1.0, 'row': 0,
				'order': 1, 'color': '#fff'}
		}
	def deviceTopology(self):
		return ''
	def newAction(self, phase, name, pid, start, end, ret, ulen):
		# new device callback for a specific phase
		self.html_device_id += 1
		devid = '%s%d' % (self.idstr, self.html_device_id)
		list = self.dmesg[phase]['list']
		length = -1.0
		if(start >= 0 and end >= 0):
			length = end - start
		i = 2
		origname = name
		while(name in list):
			name = '%s[%d]' % (origname, i)
			i += 1
		list[name] = {'name': name, 'start': start, 'end': end,
			'pid': pid, 'length': length, 'row': 0, 'id': devid,
			'ret': ret, 'ulen': ulen }
		return name
	def deviceMatch(self, pid, cg):
		if cg.end - cg.start == 0:
			return ''
		for p in data.phases:
			list = self.dmesg[p]['list']
			for devname in list:
				dev = list[devname]
				if pid != dev['pid']:
					continue
				if cg.name == 'do_one_initcall':
					if(cg.start <= dev['start'] and cg.end >= dev['end'] and dev['length'] > 0):
						dev['ftrace'] = cg
						self.do_one_initcall = True
						return devname
				else:
					if(cg.start > dev['start'] and cg.end < dev['end']):
						if 'ftraces' not in dev:
							dev['ftraces'] = []
						dev['ftraces'].append(cg)
						return devname
		return ''
	def printDetails(self):
		sysvals.vprint('Timeline Details:')
		sysvals.vprint('          Host: %s' % sysvals.hostname)
		sysvals.vprint('        Kernel: %s' % sysvals.kernel)
		sysvals.vprint('     Test time: %s' % sysvals.testtime)
		sysvals.vprint('     Boot time: %s' % self.boottime)
		for phase in self.phases:
			dc = len(self.dmesg[phase]['list'])
			sysvals.vprint('%9s mode: %.3f - %.3f (%d initcalls)' % (phase,
				self.dmesg[phase]['start']*1000,
				self.dmesg[phase]['end']*1000, dc))

# ----------------- FUNCTIONS --------------------

# Function: parseKernelLog
# Description:
#	 parse a kernel log for boot data
def parseKernelLog():
	sysvals.vprint('Analyzing the dmesg data (%s)...' % \
		os.path.basename(sysvals.dmesgfile))
	phase = 'kernel'
	data = Data(0)
	data.dmesg['kernel']['start'] = data.start = ktime = 0.0
	sysvals.stamp = {
		'time': datetime.now().strftime('%B %d %Y, %I:%M:%S %p'),
		'host': sysvals.hostname,
		'mode': 'boot', 'kernel': ''}

	tp = aslib.TestProps()
	devtemp = dict()
	if(sysvals.dmesgfile):
		lf = open(sysvals.dmesgfile, 'rb')
	else:
		lf = Popen('dmesg', stdout=PIPE).stdout
	for line in lf:
		line = aslib.ascii(line).replace('\r\n', '')
		# grab the stamp and sysinfo
		if re.match(tp.stampfmt, line):
			tp.stamp = line
			continue
		elif re.match(tp.sysinfofmt, line):
			tp.sysinfo = line
			continue
		elif re.match(tp.cmdlinefmt, line):
			tp.cmdline = line
			continue
		elif re.match(tp.kparamsfmt, line):
			tp.kparams = line
			continue
		idx = line.find('[')
		if idx > 1:
			line = line[idx:]
		m = re.match('[ \t]*(\[ *)(?P<ktime>[0-9\.]*)(\]) (?P<msg>.*)', line)
		if(not m):
			continue
		ktime = float(m.group('ktime'))
		if(ktime > 120):
			break
		msg = m.group('msg')
		data.dmesgtext.append(line)
		if(ktime == 0.0 and re.match('^Linux version .*', msg)):
			if(not sysvals.stamp['kernel']):
				sysvals.stamp['kernel'] = sysvals.kernelVersion(msg)
			continue
		m = re.match('.* setting system clock to (?P<d>[0-9\-]*)[ A-Z](?P<t>[0-9:]*) UTC.*', msg)
		if(m):
			bt = datetime.strptime(m.group('d')+' '+m.group('t'), '%Y-%m-%d %H:%M:%S')
			bt = bt - timedelta(seconds=int(ktime))
			data.boottime = bt.strftime('%Y-%m-%d_%H:%M:%S')
			sysvals.stamp['time'] = bt.strftime('%B %d %Y, %I:%M:%S %p')
			continue
		m = re.match('^calling *(?P<f>.*)\+.* @ (?P<p>[0-9]*)', msg)
		if(m):
			func = m.group('f')
			pid = int(m.group('p'))
			devtemp[func] = (ktime, pid)
			continue
		m = re.match('^initcall *(?P<f>.*)\+.* returned (?P<r>.*) after (?P<t>.*) usecs', msg)
		if(m):
			data.valid = True
			data.end = ktime
			f, r, t = m.group('f', 'r', 't')
			if(f in devtemp):
				start, pid = devtemp[f]
				data.newAction(phase, f, pid, start, ktime, int(r), int(t))
				del devtemp[f]
			continue
		if(re.match('^Freeing unused kernel .*', msg)):
			data.tUserMode = ktime
			data.dmesg['kernel']['end'] = ktime
			data.dmesg['user']['start'] = ktime
			phase = 'user'

	if tp.stamp:
		sysvals.stamp = 0
		tp.parseStamp(data, sysvals)
	data.dmesg['user']['end'] = data.end
	lf.close()
	return data

# Function: parseTraceLog
# Description:
#	 Check if trace is available and copy to a temp file
def parseTraceLog(data):
	sysvals.vprint('Analyzing the ftrace data (%s)...' % \
		os.path.basename(sysvals.ftracefile))
	# if available, calculate cgfilter allowable ranges
	cgfilter = []
	if len(sysvals.cgfilter) > 0:
		for p in data.phases:
			list = data.dmesg[p]['list']
			for i in sysvals.cgfilter:
				if i in list:
					cgfilter.append([list[i]['start']-0.0001,
						list[i]['end']+0.0001])
	# parse the trace log
	ftemp = dict()
	tp = aslib.TestProps()
	tp.setTracerType('function_graph')
	tf = open(sysvals.ftracefile, 'r')
	for line in tf:
		if line[0] == '#':
			continue
		m = re.match(tp.ftrace_line_fmt, line.strip())
		if(not m):
			continue
		m_time, m_proc, m_pid, m_msg, m_dur = \
			m.group('time', 'proc', 'pid', 'msg', 'dur')
		t = float(m_time)
		if len(cgfilter) > 0:
			allow = False
			for r in cgfilter:
				if t >= r[0] and t < r[1]:
					allow = True
					break
			if not allow:
				continue
		if t > data.end:
			break
		if(m_time and m_pid and m_msg):
			t = aslib.FTraceLine(m_time, m_msg, m_dur)
			pid = int(m_pid)
		else:
			continue
		if t.fevent or t.fkprobe:
			continue
		key = (m_proc, pid)
		if(key not in ftemp):
			ftemp[key] = []
			ftemp[key].append(aslib.FTraceCallGraph(pid, sysvals))
		cg = ftemp[key][-1]
		res = cg.addLine(t)
		if(res != 0):
			ftemp[key].append(aslib.FTraceCallGraph(pid, sysvals))
		if(res == -1):
			ftemp[key][-1].addLine(t)

	tf.close()

	# add the callgraph data to the device hierarchy
	for key in ftemp:
		proc, pid = key
		for cg in ftemp[key]:
			if len(cg.list) < 1 or cg.invalid or (cg.end - cg.start == 0):
				continue
			if(not cg.postProcess()):
				pprint('Sanity check failed for %s-%d' % (proc, pid))
				continue
			# match cg data to devices
			devname = data.deviceMatch(pid, cg)
			if not devname:
				kind = 'Orphan'
				if cg.partial:
					kind = 'Partial'
				sysvals.vprint('%s callgraph found for %s %s-%d [%f - %f]' %\
					(kind, cg.name, proc, pid, cg.start, cg.end))
			elif len(cg.list) > 1000000:
				pprint('WARNING: the callgraph found for %s is massive! (%d lines)' %\
					(devname, len(cg.list)))

# Function: retrieveLogs
# Description:
#	 Create copies of dmesg and/or ftrace for later processing
def retrieveLogs():
	# check ftrace is configured first
	if sysvals.useftrace:
		tracer = sysvals.fgetVal('current_tracer').strip()
		if tracer != 'function_graph':
			doError('ftrace not configured for a boot callgraph')
	# create the folder and get dmesg
	sysvals.systemInfo(aslib.dmidecode(sysvals.mempath))
	sysvals.initTestOutput('boot')
	sysvals.writeDatafileHeader(sysvals.dmesgfile)
	call('dmesg >> '+sysvals.dmesgfile, shell=True)
	if not sysvals.useftrace:
		return
	# get ftrace
	sysvals.writeDatafileHeader(sysvals.ftracefile)
	call('cat '+sysvals.tpath+'trace >> '+sysvals.ftracefile, shell=True)

# Function: colorForName
# Description:
#	 Generate a repeatable color from a list for a given name
def colorForName(name):
	list = [
		('c1', '#ec9999'),
		('c2', '#ffc1a6'),
		('c3', '#fff0a6'),
		('c4', '#adf199'),
		('c5', '#9fadea'),
		('c6', '#a699c1'),
		('c7', '#ad99b4'),
		('c8', '#eaffea'),
		('c9', '#dcecfb'),
		('c10', '#ffffea')
	]
	i = 0
	total = 0
	count = len(list)
	while i < len(name):
		total += ord(name[i])
		i += 1
	return list[total % count]

def cgOverview(cg, minlen):
	stats = dict()
	large = []
	for l in cg.list:
		if l.fcall and l.depth == 1:
			if l.length >= minlen:
				large.append(l)
			if l.name not in stats:
				stats[l.name] = [0, 0.0]
			stats[l.name][0] += (l.length * 1000.0)
			stats[l.name][1] += 1
	return (large, stats)

# Function: createBootGraph
# Description:
#	 Create the output html file from the resident test data
# Arguments:
#	 testruns: array of Data objects from parseKernelLog or parseTraceLog
# Output:
#	 True if the html file was created, false if it failed
def createBootGraph(data):
	# html function templates
	html_srccall = '<div id={6} title="{5}" class="srccall" style="left:{1}%;top:{2}px;height:{3}px;width:{4}%;line-height:{3}px;">{0}</div>\n'
	html_timetotal = '<table class="time1">\n<tr>'\
		'<td class="blue">Init process starts @ <b>{0} ms</b></td>'\
		'<td class="blue">Last initcall ends @ <b>{1} ms</b></td>'\
		'</tr>\n</table>\n'

	# device timeline
	devtl = aslib.Timeline(100, 20)

	# write the test title and general info header
	devtl.createHeader(sysvals, sysvals.stamp)

	# Generate the header for this timeline
	t0 = data.start
	tMax = data.end
	tTotal = tMax - t0
	if(tTotal == 0):
		pprint('ERROR: No timeline data')
		return False
	user_mode = '%.0f'%(data.tUserMode*1000)
	last_init = '%.0f'%(tTotal*1000)
	devtl.html += html_timetotal.format(user_mode, last_init)

	# determine the maximum number of rows we need to draw
	devlist = []
	for p in data.phases:
		list = data.dmesg[p]['list']
		for devname in list:
			d = aslib.DevItem(0, p, list[devname])
			devlist.append(d)
		devtl.getPhaseRows(devlist, 0, 'start')
	devtl.calcTotalRows()

	# draw the timeline background
	devtl.createZoomBox()
	devtl.html += devtl.html_tblock.format('boot', '0', '100', devtl.scaleH)
	for p in data.phases:
		phase = data.dmesg[p]
		length = phase['end']-phase['start']
		left = '%.3f' % (((phase['start']-t0)*100.0)/tTotal)
		width = '%.3f' % ((length*100.0)/tTotal)
		devtl.html += devtl.html_phase.format(left, width, \
			'%.3f'%devtl.scaleH, '%.3f'%devtl.bodyH, \
			phase['color'], '')

	# draw the device timeline
	num = 0
	devstats = dict()
	for phase in data.phases:
		list = data.dmesg[phase]['list']
		for devname in sorted(list):
			cls, color = colorForName(devname)
			dev = list[devname]
			info = '@|%.3f|%.3f|%.3f|%d' % (dev['start']*1000.0, dev['end']*1000.0,
				dev['ulen']/1000.0, dev['ret'])
			devstats[dev['id']] = {'info':info}
			dev['color'] = color
			height = devtl.phaseRowHeight(0, phase, dev['row'])
			top = '%.6f' % ((dev['row']*height) + devtl.scaleH)
			left = '%.6f' % (((dev['start']-t0)*100)/tTotal)
			width = '%.6f' % (((dev['end']-dev['start'])*100)/tTotal)
			length = ' (%0.3f ms) ' % ((dev['end']-dev['start'])*1000)
			devtl.html += devtl.html_device.format(dev['id'],
				devname+length+phase+'_mode', left, top, '%.3f'%height,
				width, devname, ' '+cls, '')
			rowtop = devtl.phaseRowTop(0, phase, dev['row'])
			height = '%.6f' % (devtl.rowH / 2)
			top = '%.6f' % (rowtop + devtl.scaleH + (devtl.rowH / 2))
			if data.do_one_initcall:
				if('ftrace' not in dev):
					continue
				cg = dev['ftrace']
				large, stats = cgOverview(cg, 0.001)
				devstats[dev['id']]['fstat'] = stats
				for l in large:
					left = '%f' % (((l.time-t0)*100)/tTotal)
					width = '%f' % (l.length*100/tTotal)
					title = '%s (%0.3fms)' % (l.name, l.length * 1000.0)
					devtl.html += html_srccall.format(l.name, left,
						top, height, width, title, 'x%d'%num)
					num += 1
				continue
			if('ftraces' not in dev):
				continue
			for cg in dev['ftraces']:
				left = '%f' % (((cg.start-t0)*100)/tTotal)
				width = '%f' % ((cg.end-cg.start)*100/tTotal)
				cglen = (cg.end - cg.start) * 1000.0
				title = '%s (%0.3fms)' % (cg.name, cglen)
				cg.id = 'x%d' % num
				devtl.html += html_srccall.format(cg.name, left,
					top, height, width, title, dev['id']+cg.id)
				num += 1

	# draw the time scale, try to make the number of labels readable
	devtl.createTimeScale(t0, tMax, tTotal, 'boot')
	devtl.html += '</div>\n'

	# timeline is finished
	devtl.html += '</div>\n</div>\n'

	# draw a legend which describes the phases by color
	devtl.html += '<div class="legend">\n'
	pdelta = 20.0
	pmargin = 36.0
	for phase in data.phases:
		order = '%.2f' % ((data.dmesg[phase]['order'] * pdelta) + pmargin)
		devtl.html += devtl.html_legend.format(order, \
			data.dmesg[phase]['color'], phase+'_mode', phase[0])
	devtl.html += '</div>\n'

	hf = open(sysvals.htmlfile, 'w')

	# add the css
	extra = '\
		.c1 {background:rgba(209,0,0,0.4);}\n\
		.c2 {background:rgba(255,102,34,0.4);}\n\
		.c3 {background:rgba(255,218,33,0.4);}\n\
		.c4 {background:rgba(51,221,0,0.4);}\n\
		.c5 {background:rgba(17,51,204,0.4);}\n\
		.c6 {background:rgba(34,0,102,0.4);}\n\
		.c7 {background:rgba(51,0,68,0.4);}\n\
		.c8 {background:rgba(204,255,204,0.4);}\n\
		.c9 {background:rgba(169,208,245,0.4);}\n\
		.c10 {background:rgba(255,255,204,0.4);}\n\
		.vt {transform:rotate(-60deg);transform-origin:0 0;}\n\
		table.fstat {table-layout:fixed;padding:150px 15px 0 0;font-size:10px;column-width:30px;}\n\
		.fstat th {width:55px;}\n\
		.fstat td {text-align:left;width:35px;}\n\
		.srccall {position:absolute;font-size:10px;z-index:7;overflow:hidden;color:black;text-align:center;white-space:nowrap;border-radius:5px;border:1px solid black;background:linear-gradient(to bottom right,#CCC,#969696);}\n\
		.srccall:hover {color:white;font-weight:bold;border:1px solid white;}\n'
	aslib.addCSS(hf, sysvals, 1, False, extra)

	# write the device timeline
	hf.write(devtl.html)

	# add boot specific html
	statinfo = 'var devstats = {\n'
	for n in sorted(devstats):
		statinfo += '\t"%s": [\n\t\t"%s",\n' % (n, devstats[n]['info'])
		if 'fstat' in devstats[n]:
			funcs = devstats[n]['fstat']
			for f in sorted(funcs, key=lambda k:(funcs[k], k), reverse=True):
				if funcs[f][0] < 0.01 and len(funcs) > 10:
					break
				statinfo += '\t\t"%f|%s|%d",\n' % (funcs[f][0], f, funcs[f][1])
		statinfo += '\t],\n'
	statinfo += '};\n'
	html = \
		'<div id="devicedetailtitle"></div>\n'\
		'<div id="devicedetail" style="display:none;">\n'\
		'<div id="devicedetail0">\n'
	for p in data.phases:
		phase = data.dmesg[p]
		html += devtl.html_phaselet.format(p+'_mode', '0', '100', phase['color'])
	html += '</div>\n</div>\n'\
		'<script type="text/javascript">\n'+statinfo+\
		'</script>\n'
	hf.write(html)

	# add the callgraph html
	if(sysvals.usecallgraph):
		aslib.addCallgraphs(sysvals, hf, data)

	# add the test log as a hidden div
	if sysvals.testlog and sysvals.logmsg:
		hf.write('<div id="testlog" style="display:none;">\n'+sysvals.logmsg+'</div>\n')
	# add the dmesg log as a hidden div
	if sysvals.dmesglog:
		hf.write('<div id="dmesglog" style="display:none;">\n')
		for line in data.dmesgtext:
			line = line.replace('<', '&lt').replace('>', '&gt')
			hf.write(line)
		hf.write('</div>\n')

	# write the footer and close
	aslib.addScriptCode(hf, [data])
	hf.write('</body>\n</html>\n')
	hf.close()
	return True

# Function: updateCron
# Description:
#    (restore=False) Set the tool to run automatically on reboot
#    (restore=True) Restore the original crontab
def updateCron(restore=False):
	if not restore:
		sysvals.rootUser(True)
	crondir = '/var/spool/cron/crontabs/'
	if not os.path.exists(crondir):
		crondir = '/var/spool/cron/'
	if not os.path.exists(crondir):
		doError('%s not found' % crondir)
	cronfile = crondir+'root'
	backfile = crondir+'root-analyze_boot-backup'
	cmd = sysvals.getExec('crontab')
	if not cmd:
		doError('crontab not found')
	# on restore: move the backup cron back into place
	if restore:
		if os.path.exists(backfile):
			shutil.move(backfile, cronfile)
			call([cmd, cronfile])
		return
	# backup current cron and install new one with reboot
	if os.path.exists(cronfile):
		shutil.move(cronfile, backfile)
	else:
		fp = open(backfile, 'w')
		fp.close()
	res = -1
	try:
		fp = open(backfile, 'r')
		op = open(cronfile, 'w')
		for line in fp:
			if not sysvals.myCronJob(line):
				op.write(line)
				continue
		fp.close()
		op.write('@reboot python %s\n' % sysvals.cronjobCmdString())
		op.close()
		res = call([cmd, cronfile])
	except Exception as e:
		pprint('Exception: %s' % str(e))
		shutil.move(backfile, cronfile)
		res = -1
	if res != 0:
		doError('crontab failed')

# Function: updateGrub
# Description:
#	 update grub.cfg for all kernels with our parameters
def updateGrub(restore=False):
	# call update-grub on restore
	if restore:
		try:
			call(sysvals.blexec, stderr=PIPE, stdout=PIPE,
				env={'PATH': '.:/sbin:/usr/sbin:/usr/bin:/sbin:/bin'})
		except Exception as e:
			pprint('Exception: %s\n' % str(e))
		return
	# extract the option and create a grub config without it
	sysvals.rootUser(True)
	tgtopt = 'GRUB_CMDLINE_LINUX_DEFAULT'
	cmdline = ''
	grubfile = '/etc/default/grub'
	tempfile = '/etc/default/grub.analyze_boot'
	shutil.move(grubfile, tempfile)
	res = -1
	try:
		fp = open(tempfile, 'r')
		op = open(grubfile, 'w')
		cont = False
		for line in fp:
			line = line.strip()
			if len(line) == 0 or line[0] == '#':
				continue
			opt = line.split('=')[0].strip()
			if opt == tgtopt:
				cmdline = line.split('=', 1)[1].strip('\\')
				if line[-1] == '\\':
					cont = True
			elif cont:
				cmdline += line.strip('\\')
				if line[-1] != '\\':
					cont = False
			else:
				op.write('%s\n' % line)
		fp.close()
		# if the target option value is in quotes, strip them
		sp = '"'
		val = cmdline.strip()
		if val and (val[0] == '\'' or val[0] == '"'):
			sp = val[0]
			val = val.strip(sp)
		cmdline = val
		# append our cmd line options
		if len(cmdline) > 0:
			cmdline += ' '
		cmdline += sysvals.kernelParams()
		# write out the updated target option
		op.write('\n%s=%s%s%s\n' % (tgtopt, sp, cmdline, sp))
		op.close()
		res = call(sysvals.blexec)
		os.remove(grubfile)
	except Exception as e:
		pprint('Exception: %s' % str(e))
		res = -1
	# cleanup
	shutil.move(tempfile, grubfile)
	if res != 0:
		doError('update grub failed')

# Function: updateKernelParams
# Description:
#	 update boot conf for all kernels with our parameters
def updateKernelParams(restore=False):
	# find the boot loader
	sysvals.getBootLoader()
	if sysvals.bootloader == 'grub':
		updateGrub(restore)

# Function: doError Description:
#	 generic error function for catastrphic failures
# Arguments:
#	 msg: the error message to print
#	 help: True if printHelp should be called after, False otherwise
def doError(msg, help=False):
	if help == True:
		printHelp()
	pprint('ERROR: %s\n' % msg)
	sysvals.outputResult({'error':msg})
	sys.exit()

# Function: printHelp
# Description:
#	 print out the help text
def printHelp():
	pprint('\n%s v%s\n'\
	'Usage: bootgraph <options> <command>\n'\
	'\n'\
	'Description:\n'\
	'  This tool reads in a dmesg log of linux kernel boot and\n'\
	'  creates an html representation of the boot timeline up to\n'\
	'  the start of the init process.\n'\
	'\n'\
	'  If no specific command is given the tool reads the current dmesg\n'\
	'  and/or ftrace log and creates a timeline\n'\
	'\n'\
	'  Generates output files in subdirectory: boot-yymmdd-HHMMSS\n'\
	'   HTML output:                    <hostname>_boot.html\n'\
	'   raw dmesg output:               <hostname>_boot_dmesg.txt\n'\
	'   raw ftrace output:              <hostname>_boot_ftrace.txt\n'\
	'\n'\
	'Options:\n'\
	'  -h            Print this help text\n'\
	'  -v            Print the current tool version\n'\
	'  -verbose      Print extra information during execution and analysis\n'\
	'  -addlogs      Add the dmesg log to the html output\n'\
	'  -result fn    Export a results table to a text file for parsing.\n'\
	'  -o name       Overrides the output subdirectory name when running a new test\n'\
	'                default: boot-{date}-{time}\n'\
	' [advanced]\n'\
	'  -fstat        Use ftrace to add function detail and statistics (default: disabled)\n'\
	'  -f/-callgraph Add callgraph detail, can be very large (default: disabled)\n'\
	'  -maxdepth N   limit the callgraph data to N call levels (default: 2)\n'\
	'  -mincg ms     Discard all callgraphs shorter than ms milliseconds (e.g. 0.001 for us)\n'\
	'  -timeprec N   Number of significant digits in timestamps (0:S, 3:ms, [6:us])\n'\
	'  -expandcg     pre-expand the callgraph data in the html output (default: disabled)\n'\
	'  -func list    Limit ftrace to comma-delimited list of functions (default: do_one_initcall)\n'\
	'  -cgfilter S   Filter the callgraph output in the timeline\n'\
	'  -cgskip file  Callgraph functions to skip, off to disable (default: cgskip.txt)\n'\
	'  -bl name      Use the following boot loader for kernel params (default: grub)\n'\
	'  -reboot       Reboot the machine automatically and generate a new timeline\n'\
	'  -manual       Show the steps to generate a new timeline manually (used with -reboot)\n'\
	'\n'\
	'Other commands:\n'\
	'  -flistall     Print all functions capable of being captured in ftrace\n'\
	'  -sysinfo      Print out system info extracted from BIOS\n'\
	'  -which exec   Print an executable path, should function even without PATH\n'\
	' [redo]\n'\
	'  -dmesg file   Create HTML output using dmesg input (used with -ftrace)\n'\
	'  -ftrace file  Create HTML output using ftrace input (used with -dmesg)\n'\
	'' % (sysvals.title, sysvals.version))
	return True

# ----------------- MAIN --------------------
# exec start (skipped if script is loaded as library)
if __name__ == '__main__':
	# loop through the command line arguments
	cmd = ''
	testrun = True
	switchoff = ['disable', 'off', 'false', '0']
	simplecmds = ['-sysinfo', '-kpupdate', '-flistall', '-checkbl']
	cgskip = ''
	if '-f' in sys.argv:
		cgskip = sysvals.configFile('cgskip.txt')
	args = iter(sys.argv[1:])
	mdset = False
	for arg in args:
		if(arg == '-h'):
			printHelp()
			sys.exit()
		elif(arg == '-v'):
			pprint("Version %s" % sysvals.version)
			sys.exit()
		elif(arg == '-verbose'):
			sysvals.verbose = True
		elif(arg in simplecmds):
			cmd = arg[1:]
		elif(arg == '-fstat'):
			sysvals.useftrace = True
		elif(arg == '-callgraph' or arg == '-f'):
			sysvals.useftrace = True
			sysvals.usecallgraph = True
		elif(arg == '-cgdump'):
			sysvals.cgdump = True
		elif(arg == '-mincg'):
			sysvals.mincglen = aslib.getArgFloat('-mincg', args, 0.0, 10000.0)
		elif(arg == '-cgfilter'):
			try:
				val = next(args)
			except:
				doError('No callgraph functions supplied', True)
			sysvals.setCallgraphFilter(val)
		elif(arg == '-cgskip'):
			try:
				val = next(args)
			except:
				doError('No file supplied', True)
			if val.lower() in switchoff:
				cgskip = ''
			else:
				cgskip = sysvals.configFile(val)
				if(not cgskip):
					doError('%s does not exist' % cgskip)
		elif(arg == '-bl'):
			try:
				val = next(args)
			except:
				doError('No boot loader name supplied', True)
			if val.lower() not in ['grub']:
				doError('Unknown boot loader: %s' % val, True)
			sysvals.bootloader = val.lower()
		elif(arg == '-timeprec'):
			sysvals.setPrecision(aslib.getArgInt('-timeprec', args, 0, 6))
		elif(arg == '-maxdepth'):
			mdset = True
			sysvals.max_graph_depth = aslib.getArgInt('-maxdepth', args, 0, 1000)
		elif(arg == '-func'):
			try:
				val = next(args)
			except:
				doError('No filter functions supplied', True)
			sysvals.useftrace = True
			sysvals.usecallgraph = True
			sysvals.rootCheck(True)
			sysvals.setGraphFilter(val)
		elif(arg == '-ftrace'):
			try:
				val = next(args)
			except:
				doError('No ftrace file supplied', True)
			if(os.path.exists(val) == False):
				doError('%s does not exist' % val)
			testrun = False
			sysvals.ftracefile = val
		elif(arg == '-addlogs'):
			sysvals.dmesglog = True
		elif(arg == '-expandcg'):
			sysvals.cgexp = True
		elif(arg == '-dmesg'):
			try:
				val = next(args)
			except:
				doError('No dmesg file supplied', True)
			if(os.path.exists(val) == False):
				doError('%s does not exist' % val)
			testrun = False
			sysvals.dmesgfile = val
		elif(arg == '-o'):
			try:
				val = next(args)
			except:
				doError('No subdirectory name supplied', True)
			sysvals.testdir = sysvals.setOutputFolder(val)
		elif(arg == '-result'):
			try:
				val = next(args)
			except:
				doError('No result file supplied', True)
			sysvals.result = val
		elif(arg == '-reboot'):
			sysvals.reboot = True
		elif(arg == '-manual'):
			sysvals.reboot = True
			sysvals.manual = True
		# remaining options are only for cron job use
		elif(arg == '-cronjob'):
			sysvals.iscronjob = True
		elif(arg == '-which'):
			try:
				val = next(args)
			except:
				doError('No executable supplied', True)
			out = sysvals.getExec(val)
			if not out:
				print('%s not found' % val)
				sys.exit(1)
			print(out)
			sys.exit(0)
		else:
			doError('Invalid argument: '+arg, True)

	# compatibility errors and access checks
	if(sysvals.iscronjob and (sysvals.reboot or \
		sysvals.dmesgfile or sysvals.ftracefile or cmd)):
		doError('-cronjob is meant for batch purposes only')
	if(sysvals.reboot and (sysvals.dmesgfile or sysvals.ftracefile)):
		doError('-reboot and -dmesg/-ftrace are incompatible')
	if cmd or sysvals.reboot or sysvals.iscronjob or testrun:
		sysvals.rootCheck(True)
	if (testrun and sysvals.useftrace) or cmd == 'flistall':
		if not sysvals.verifyFtrace():
			doError('Ftrace is not properly enabled')

	# run utility commands
	sysvals.cpuInfo()
	if cmd != '':
		if cmd == 'kpupdate':
			updateKernelParams()
		elif cmd == 'flistall':
			for f in sysvals.getBootFtraceFilterFunctions():
				print(f)
		elif cmd == 'checkbl':
			sysvals.getBootLoader()
			pprint('Boot Loader: %s\n%s' % (sysvals.bootloader, sysvals.blexec))
		elif(cmd == 'sysinfo'):
			sysvals.printSystemInfo(True)
		sys.exit()

	# reboot: update grub, setup a cronjob, and reboot
	if sysvals.reboot:
		if (sysvals.useftrace or sysvals.usecallgraph) and \
			not sysvals.checkFtraceKernelVersion():
			doError('Ftrace functionality requires kernel v4.10 or newer')
		if not sysvals.manual:
			updateKernelParams()
			updateCron()
			call('reboot')
		else:
			sysvals.manualRebootRequired()
		sys.exit()

	if sysvals.usecallgraph and cgskip:
		sysvals.vprint('Using cgskip file: %s' % cgskip)
		sysvals.setCallgraphBlacklist(cgskip)

	# cronjob: remove the cronjob, grub changes, and disable ftrace
	if sysvals.iscronjob:
		updateCron(True)
		updateKernelParams(True)
		try:
			sysvals.fsetVal('0', 'tracing_on')
		except:
			pass

	# testrun: generate copies of the logs
	if testrun:
		retrieveLogs()
	else:
		sysvals.setOutputFile()

	# process the log data
	if sysvals.dmesgfile:
		if not mdset:
			sysvals.max_graph_depth = 0
		data = parseKernelLog()
		if(not data.valid):
			doError('No initcall data found in %s' % sysvals.dmesgfile)
		if sysvals.useftrace and sysvals.ftracefile:
			parseTraceLog(data)
		if sysvals.cgdump:
			data.debugPrint()
			sys.exit()
	else:
		doError('dmesg file required')

	sysvals.vprint('Creating the html timeline (%s)...' % sysvals.htmlfile)
	sysvals.vprint('Command:\n    %s' % sysvals.cmdline)
	sysvals.vprint('Kernel parameters:\n    %s' % sysvals.kparams)
	data.printDetails()
	createBootGraph(data)

	# if running as root, change output dir owner to sudo_user
	if testrun and os.path.isdir(sysvals.testdir) and \
		os.getuid() == 0 and 'SUDO_USER' in os.environ:
		cmd = 'chown -R {0}:{0} {1} > /dev/null 2>&1'
		call(cmd.format(os.environ['SUDO_USER'], sysvals.testdir), shell=True)

	sysvals.stamp['boot'] = (data.tUserMode - data.start) * 1000
	sysvals.stamp['lastinit'] = data.end * 1000
	sysvals.outputResult(sysvals.stamp)
