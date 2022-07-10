# c-container
## A rough container in C

Download Alpine root filesystem into `alpine-minirootfs`.

```bash
mkdir -p alpine-minirootfs && wget -qO- https://dl-cdn.alpinelinux.org/alpine/v3.16/releases/x86_64/alpine-minirootfs-3.16.0-x86_64.tar.gz | tar zxv -C alpine-minirootfs
```

Compile `c-container.c`.

```bash
gcc c-container.c -o c-container
```

Run it as root.

```bash
sudo ./c-container
```

Test it.

```bash
root@container$ id
uid=0(root) gid=0(root) groups=0(root)

root@container$ ps
PID   USER     TIME  COMMAND
    1 root      0:00 /bin/sh
    2 root      0:00 ps

root@container$ readlink /proc/$$/ns/pid
pid:[4026533802]

root@container$ readlink /proc/$$/ns/mnt
mnt:[4026533799]

root@container$ readlink /proc/$$/ns/uts
uts:[4026533800]

```
