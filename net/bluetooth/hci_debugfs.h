/*
   BlueZ - Bluetooth protocol stack for Linux
   Copyright (C) 2014 Intel Corporation

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS
   SOFTWARE IS DISCLAIMED.
*/

#if IS_ENABLED(CONFIG_BT_DEBUGFS)

void hci_debugfs_create_common(struct hci_dev *hdev);
void hci_debugfs_create_bredr(struct hci_dev *hdev);
void hci_debugfs_create_le(struct hci_dev *hdev);
void hci_debugfs_create_conn(struct hci_conn *conn);

#else

static inline void hci_debugfs_create_common(struct hci_dev *hdev)
{
}

static inline void hci_debugfs_create_bredr(struct hci_dev *hdev)
{
}

static inline void hci_debugfs_create_le(struct hci_dev *hdev)
{
}

static inline void hci_debugfs_create_conn(struct hci_conn *conn)
{
}

#endif
