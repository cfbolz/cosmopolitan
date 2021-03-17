/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates a pair of connected sockets
 *
 * @param family can be AF_UNIX, AF_INET, etc.
 * @param type can be SOCK_STREAM (for TCP), SOCK_DGRAM (e.g. UDP), or
 *     SOCK_RAW (IP) so long as IP_HDRINCL was passed to setsockopt();
 *     and additionally, may be or'd with SOCK_NONBLOCK, SOCK_CLOEXEC
 * @param protocol can be IPPROTO_TCP, IPPROTO_UDP, or IPPROTO_ICMP
 * @param sv a vector of 2 integers to store the created sockets.
 * @return 0 if success, -1 in case of error
 * @error EFAULT, EPFNOSUPPORT, etc.
 * @see libc/sysv/consts.sh
 * @asyncsignalsafe
 */
int socketpair(int family, int type, int protocol, int sv[2]) {
  if (family == AF_UNSPEC) {
    family = AF_UNIX;
  } else if (family == AF_INET6) {
    /* Recommend IPv6 on frontend serving infrastructure only. That's
       what Google Cloud does. It's more secure. It also means poll()
       will work on Windows, which doesn't allow mixing third layers. */
    errno = EAFNOSUPPORT;
    return epfnosupport();
  }
  if (!IsWindows()) {
    return sys_socketpair(family, type, protocol, sv);
  }
  return sys_socketpair_nt(family, type, protocol, sv);
}