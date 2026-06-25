/* nservers.h — NovaHost network servers for the NovaVM Linux host.
 *
 * Userspace (BSD sockets) port of the bare-metal ps_fio lwIP servers:
 *   - net.c   : 6502 raw file-upload server   -> /data/nova/UPLOAD.NDI
 *   - debug.c : 6503 newline-delimited JSON debug server
 *   - mgmt.c  : 6504 NVH1 length-framed CBOR management server
 *
 * servers_init() spawns one detached pthread per port (blocking accept loops,
 * one client at a time, matching the bare-metal request/response model). The
 * servers reach the machine through novavm.h's poke/peek/wr/rd primitives, so
 * the `nova` CLI / NovaPanel drive this box exactly like the ULX3S/bare-metal
 * box does with `--remote <ip>`.
 *
 * Call servers_init() once from novavm.c, after the 6502 is released and the
 * FIO service loop is about to start (the servers run on their own threads). */
#ifndef NSERVERS_H
#define NSERVERS_H

void servers_init(void);   /* spawn the 6502 / 6503 / 6504 listener threads */

#endif /* NSERVERS_H */
