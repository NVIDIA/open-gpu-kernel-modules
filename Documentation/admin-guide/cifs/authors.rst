=======
Authors
=======

Original Author
---------------

Steve French (smfrench@gmail.com, sfrench@samba.org)

The author wishes to express his appreciation and thanks to:
Andrew Tridgell (Samba team) for his early suggestions about SMB/CIFS VFS
improvements. Thanks to IBM for allowing me time and test resources to pursue
this project, to Jim McDonough from IBM (and the Samba Team) for his help, to
the IBM Linux JFS team for explaining many esoteric Linux filesystem features.
Jeremy Allison of the Samba team has done invaluable work in adding the server
side of the original CIFS Unix extensions and reviewing and implementing
portions of the newer CIFS POSIX extensions into the Samba 3 file server. Thank
Dave Boutcher of IBM Rochester (author of the OS/400 smb/cifs filesystem client)
for proving years ago that very good smb/cifs clients could be done on Unix-like
operating systems.  Volker Lendecke, Andrew Tridgell, Urban Widmark, John
Newbigin and others for their work on the Linux smbfs module.  Thanks to
the other members of the Storage Network Industry Association CIFS Technical
Workgroup for their work specifying this highly complex protocol and finally
thanks to the Samba team for their technical advice and encouragement.

Patch Contributors
------------------

- Zwane Mwaikambo
- Andi Kleen
- Amrut Joshi
- Shobhit Dayal
- Sergey Vlasov
- Richard Hughes
- Yury Umanets
- Mark Hamzy (for some of the early cifs IPv6 work)
- Domen Puncer
- Jesper Juhl (in particular for lots of whitespace/formatting cleanup)
- Vince Negri and Dave Stahl (for finding an important caching bug)
- Adrian Bunk (kcalloc cleanups)
- Miklos Szeredi
- Kazeon team for various fixes especially for 2.4 version.
- Asser Ferno (Change Notify support)
- Shaggy (Dave Kleikamp) for innumerable small fs suggestions and some good cleanup
- Gunter Kukkukk (testing and suggestions for support of old servers)
- Igor Mammedov (DFS support)
- Jeff Layton (many, many fixes, as well as great work on the cifs Kerberos code)
- Scott Lovenberg
- Pavel Shilovsky (for great work adding SMB2 support, and various SMB3 features)
- Aurelien Aptel (for DFS SMB3 work and some key bug fixes)
- Ronnie Sahlberg (for SMB3 xattr work, bug fixes, and lots of great work on compounding)
- Shirish Pargaonkar (for many ACL patches over the years)
- Sachin Prabhu (many bug fixes, including for reconnect, copy offload and security)
- Paulo Alcantara (for some excellent work in DFS, and in booting from SMB3)
- Long Li (some great work on RDMA, SMB Direct)


Test case and Bug Report contributors
-------------------------------------
Thanks to those in the community who have submitted detailed bug reports
and debug of problems they have found:  Jochen Dolze, David Blaine,
Rene Scharfe, Martin Josefsson, Alexander Wild, Anthony Liguori,
Lars Muller, Urban Widmark, Massimiliano Ferrero, Howard Owen,
Olaf Kirch, Kieron Briggs, Nick Millington and others. Also special
mention to the Stanford Checker (SWAT) which pointed out many minor
bugs in error paths.  Valuable suggestions also have come from Al Viro
and Dave Miller.

And thanks to the IBM LTC and Power test teams and SuSE and Citrix and RedHat testers for finding multiple bugs during excellent stress test runs.
