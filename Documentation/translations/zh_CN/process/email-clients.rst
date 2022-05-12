﻿.. _cn_email_clients:

.. include:: ../disclaimer-zh_CN.rst

:Original: :ref:`Documentation/process/email-clients.rst <email_clients>`

译者::

        中文版维护者： 贾威威  Harry Wei <harryxiyou@gmail.com>
        中文版翻译者： 贾威威  Harry Wei <harryxiyou@gmail.com>
                       时奎亮  Alex Shi <alex.shi@linux.alibaba.com>
        中文版校译者： Yinglin Luan <synmyth@gmail.com>
        	       Xiaochen Wang <wangxiaochen0@gmail.com>
                       yaxinsn <yaxinsn@163.com>

Linux邮件客户端配置信息
=======================

Git
---

现在大多数开发人员使用 ``git send-email`` 而不是常规的电子邮件客户端。这方面
的手册非常好。在接收端，维护人员使用 ``git am`` 加载补丁。

如果你是 ``git`` 新手，那么把你的第一个补丁发送给你自己。将其保存为包含所有
标题的原始文本。运行 ``git am raw_email.txt`` ，然后使用 ``git log`` 查看更
改日志。如果工作正常，再将补丁发送到相应的邮件列表。


普通配置
--------
Linux内核补丁是通过邮件被提交的，最好把补丁作为邮件体的内嵌文本。有些维护者
接收附件，但是附件的内容格式应该是"text/plain"。然而，附件一般是不赞成的，
因为这会使补丁的引用部分在评论过程中变的很困难。

用来发送Linux内核补丁的邮件客户端在发送补丁时应该处于文本的原始状态。例如，
他们不能改变或者删除制表符或者空格，甚至是在每一行的开头或者结尾。

不要通过"format=flowed"模式发送补丁。这样会引起不可预期以及有害的断行。

不要让你的邮件客户端进行自动换行。这样也会破坏你的补丁。

邮件客户端不能改变文本的字符集编码方式。要发送的补丁只能是ASCII或者UTF-8编码方式，
如果你使用UTF-8编码方式发送邮件，那么你将会避免一些可能发生的字符集问题。

邮件客户端应该形成并且保持 References: 或者 In-Reply-To: 标题，那么
邮件话题就不会中断。

复制粘帖(或者剪贴粘帖)通常不能用于补丁，因为制表符会转换为空格。使用xclipboard, xclip
或者xcutsel也许可以，但是最好测试一下或者避免使用复制粘帖。

不要在使用PGP/GPG署名的邮件中包含补丁。这样会使得很多脚本不能读取和适用于你的补丁。
（这个问题应该是可以修复的）

在给内核邮件列表发送补丁之前，给自己发送一个补丁是个不错的主意，保存接收到的
邮件，将补丁用'patch'命令打上，如果成功了，再给内核邮件列表发送。


一些邮件客户端提示
------------------
这里给出一些详细的MUA配置提示，可以用于给Linux内核发送补丁。这些并不意味是
所有的软件包配置总结。

说明：
TUI = 以文本为基础的用户接口
GUI = 图形界面用户接口

Alpine (TUI)
~~~~~~~~~~~~

配置选项：
在"Sending Preferences"部分：

- "Do Not Send Flowed Text"必须开启
- "Strip Whitespace Before Sending"必须关闭

当写邮件时，光标应该放在补丁会出现的地方，然后按下CTRL-R组合键，使指定的
补丁文件嵌入到邮件中。

Evolution (GUI)
~~~~~~~~~~~~~~~

一些开发者成功的使用它发送补丁

当选择邮件选项：Preformat
  从Format->Heading->Preformatted (Ctrl-7)或者工具栏

然后使用：
  Insert->Text File... (Alt-n x)插入补丁文件。

你还可以"diff -Nru old.c new.c | xclip"，选择Preformat，然后使用中间键进行粘帖。

Kmail (GUI)
~~~~~~~~~~~

一些开发者成功的使用它发送补丁。

默认设置不为HTML格式是合适的；不要启用它。

当书写一封邮件的时候，在选项下面不要选择自动换行。唯一的缺点就是你在邮件中输入的任何文本
都不会被自动换行，因此你必须在发送补丁之前手动换行。最简单的方法就是启用自动换行来书写邮件，
然后把它保存为草稿。一旦你在草稿中再次打开它，它已经全部自动换行了，那么你的邮件虽然没有
选择自动换行，但是还不会失去已有的自动换行。

在邮件的底部，插入补丁之前，放上常用的补丁定界符：三个连字号(---)。

然后在"Message"菜单条目，选择插入文件，接着选取你的补丁文件。还有一个额外的选项，你可以
通过它配置你的邮件建立工具栏菜单，还可以带上"insert file"图标。

你可以安全地通过GPG标记附件，但是内嵌补丁最好不要使用GPG标记它们。作为内嵌文本的签发补丁，
当从GPG中提取7位编码时会使他们变的更加复杂。

如果你非要以附件的形式发送补丁，那么就右键点击附件，然后选中属性，突出"Suggest automatic
display"，这样内嵌附件更容易让读者看到。

当你要保存将要发送的内嵌文本补丁，你可以从消息列表窗格选择包含补丁的邮件，然后右击选择
"save as"。你可以使用一个没有更改的包含补丁的邮件，如果它是以正确的形式组成。当你正真在它
自己的窗口之下察看，那时没有选项可以保存邮件--已经有一个这样的bug被汇报到了kmail的bugzilla
并且希望这将会被处理。邮件是以只针对某个用户可读写的权限被保存的，所以如果你想把邮件复制到其他地方，
你不得不把他们的权限改为组或者整体可读。

Lotus Notes (GUI)
~~~~~~~~~~~~~~~~~

不要使用它。

Mutt (TUI)
~~~~~~~~~~

很多Linux开发人员使用mutt客户端，所以证明它肯定工作的非常漂亮。

Mutt不自带编辑器，所以不管你使用什么编辑器都不应该带有自动断行。大多数编辑器都带有
一个"insert file"选项，它可以通过不改变文件内容的方式插入文件。

'vim'作为mutt的编辑器：
  set editor="vi"

  如果使用xclip，敲入以下命令
  :set paste
  按中键之前或者shift-insert或者使用
  :r filename

如果想要把补丁作为内嵌文本。
(a)ttach工作的很好，不带有"set paste"。

你可以通过 ``git format-patch`` 生成补丁，然后用 Mutt发送它们::

        $ mutt -H 0001-some-bug-fix.patch

配置选项：
它应该以默认设置的形式工作。
然而，把"send_charset"设置为"us-ascii::utf-8"也是一个不错的主意。

Mutt 是高度可配置的。 这里是个使用mutt通过 Gmail 发送的补丁的最小配置::

  # .muttrc
  # ================  IMAP ====================
  set imap_user = 'yourusername@gmail.com'
  set imap_pass = 'yourpassword'
  set spoolfile = imaps://imap.gmail.com/INBOX
  set folder = imaps://imap.gmail.com/
  set record="imaps://imap.gmail.com/[Gmail]/Sent Mail"
  set postponed="imaps://imap.gmail.com/[Gmail]/Drafts"
  set mbox="imaps://imap.gmail.com/[Gmail]/All Mail"

  # ================  SMTP  ====================
  set smtp_url = "smtp://username@smtp.gmail.com:587/"
  set smtp_pass = $imap_pass
  set ssl_force_tls = yes # Require encrypted connection

  # ================  Composition  ====================
  set editor = `echo \$EDITOR`
  set edit_headers = yes  # See the headers when editing
  set charset = UTF-8     # value of $LANG; also fallback for send_charset
  # Sender, email address, and sign-off line must match
  unset use_domain        # because joe@localhost is just embarrassing
  set realname = "YOUR NAME"
  set from = "username@gmail.com"
  set use_from = yes

Mutt文档含有更多信息:

    http://dev.mutt.org/trac/wiki/UseCases/Gmail

    http://dev.mutt.org/doc/manual.html

Pine (TUI)
~~~~~~~~~~

Pine过去有一些空格删减问题，但是这些现在应该都被修复了。

如果可以，请使用alpine(pine的继承者)

配置选项：
- 最近的版本需要消除流程文本
- "no-strip-whitespace-before-send"选项也是需要的。


Sylpheed (GUI)
~~~~~~~~~~~~~~

- 内嵌文本可以很好的工作（或者使用附件）。
- 允许使用外部的编辑器。
- 对于目录较多时非常慢。
- 如果通过non-SSL连接，无法使用TLS SMTP授权。
- 在组成窗口中有一个很有用的ruler bar。
- 给地址本中添加地址就不会正确的了解显示名。

Thunderbird (GUI)
~~~~~~~~~~~~~~~~~

默认情况下，thunderbird很容易损坏文本，但是还有一些方法可以强制它变得更好。

- 在用户帐号设置里，组成和寻址，不要选择"Compose messages in HTML format"。

- 编辑你的Thunderbird配置设置来使它不要拆行使用：user_pref("mailnews.wraplength", 0);

- 编辑你的Thunderbird配置设置，使它不要使用"format=flowed"格式：user_pref("mailnews.
  send_plaintext_flowed", false);

- 你需要使Thunderbird变为预先格式方式：
  如果默认情况下你书写的是HTML格式，那不是很难。仅仅从标题栏的下拉框中选择"Preformat"格式。
  如果默认情况下你书写的是文本格式，你不得把它改为HTML格式（仅仅作为一次性的）来书写新的消息，
  然后强制使它回到文本格式，否则它就会拆行。要实现它，在写信的图标上使用shift键来使它变为HTML
  格式，然后标题栏的下拉框中选择"Preformat"格式。

- 允许使用外部的编辑器：
  针对Thunderbird打补丁最简单的方法就是使用一个"external editor"扩展，然后使用你最喜欢的
  $EDITOR来读取或者合并补丁到文本中。要实现它，可以下载并且安装这个扩展，然后添加一个使用它的
  按键View->Toolbars->Customize...最后当你书写信息的时候仅仅点击它就可以了。

TkRat (GUI)
~~~~~~~~~~~

可以使用它。使用"Insert file..."或者外部的编辑器。

Gmail (Web GUI)
~~~~~~~~~~~~~~~

不要使用它发送补丁。

Gmail网页客户端自动地把制表符转换为空格。

虽然制表符转换为空格问题可以被外部编辑器解决，同时它还会使用回车换行把每行拆分为78个字符。

另一个问题是Gmail还会把任何不是ASCII的字符的信息改为base64编码。它把东西变的像欧洲人的名字。

                                ###
