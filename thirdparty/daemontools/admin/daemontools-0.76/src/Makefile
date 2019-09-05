default: it

clean:
	rm -f alloc.o alloc_re.o buffer.o buffer_0.o buffer_1.o buffer_2.o \
	buffer_get.o buffer_put.o buffer_read.o buffer_write.o byte.a \
	byte_chr.o byte_copy.o byte_cr.o byte_diff.o byte_rchr.o chkshsgr \
	chkshsgr.o choose coe.o compile deepsleep.o direntry.h env.o envdir \
	envdir.o envuidgid envuidgid.o error.o error_str.o fd_copy.o \
	fd_move.o fghack fghack.o fifo.o fmt_uint.o fmt_uint0.o fmt_ulong.o \
	hasflock.h hasmkffo.h hassgact.h hassgprm.h hasshsgr.h haswaitp.h \
	iopause.h iopause.o it load lock_ex.o lock_exnb.o makelib match.o \
	matchtest matchtest.o multilog multilog.o ndelay_off.o ndelay_on.o \
	open_append.o open_read.o open_trunc.o open_write.o openreadclose.o \
	pathexec_env.o pathexec_run.o pgrphack pgrphack.o prot.o readclose.o \
	readproctitle readproctitle.o rts scan_ulong.o seek_set.o select.h \
	setlock setlock.o setuidgid setuidgid.o sgetopt.o sig.o sig_block.o \
	sig_catch.o sig_pause.o softlimit softlimit.o str_chr.o str_diff.o \
	str_len.o str_start.o stralloc_cat.o stralloc_catb.o stralloc_cats.o \
	stralloc_eady.o stralloc_opyb.o stralloc_opys.o stralloc_pend.o \
	strerr_die.o strerr_sys.o subgetopt.o supervise supervise.o svc svc.o \
	svok svok.o svscan svscan.o svscanboot svstat svstat.o sysdeps \
	systype tai64n tai64n.o tai64nlocal tai64nlocal.o tai_now.o \
	tai_pack.o tai_sub.o tai_unpack.o taia_add.o taia_approx.o \
	taia_frac.o taia_less.o taia_now.o taia_pack.o taia_sub.o taia_uint.o \
	time.a timestamp.o uint64.h unix.a wait_nohang.o wait_pid.o

alloc.o: alloc.c alloc.h compile error.h
	./compile alloc.c

alloc_re.o: alloc.h alloc_re.c byte.h compile
	./compile alloc_re.c

buffer.o: buffer.c buffer.h compile
	./compile buffer.c

buffer_0.o: buffer.h buffer_0.c compile
	./compile buffer_0.c

buffer_1.o: buffer.h buffer_1.c compile
	./compile buffer_1.c

buffer_2.o: buffer.h buffer_2.c compile
	./compile buffer_2.c

buffer_get.o: buffer.h buffer_get.c byte.h compile error.h
	./compile buffer_get.c

buffer_put.o: buffer.h buffer_put.c byte.h compile error.h str.h
	./compile buffer_put.c

buffer_read.o: buffer.h buffer_read.c compile
	./compile buffer_read.c

buffer_write.o: buffer.h buffer_write.c compile
	./compile buffer_write.c

byte.a: byte_chr.o byte_copy.o byte_cr.o byte_diff.o byte_rchr.o \
fmt_uint.o fmt_uint0.o fmt_ulong.o makelib scan_ulong.o str_chr.o \
str_diff.o str_len.o str_start.o
	./makelib byte.a byte_chr.o byte_copy.o byte_cr.o byte_diff.o \
	byte_rchr.o fmt_uint.o fmt_uint0.o fmt_ulong.o scan_ulong.o str_chr.o \
	str_diff.o str_len.o str_start.o

byte_chr.o: byte.h byte_chr.c compile
	./compile byte_chr.c

byte_copy.o: byte.h byte_copy.c compile
	./compile byte_copy.c

byte_cr.o: byte.h byte_cr.c compile
	./compile byte_cr.c

byte_diff.o: byte.h byte_diff.c compile
	./compile byte_diff.c

byte_rchr.o: byte.h byte_rchr.c compile
	./compile byte_rchr.c

chkshsgr: chkshsgr.o load
	./load chkshsgr 

chkshsgr.o: chkshsgr.c compile
	./compile chkshsgr.c

choose: choose.sh home warn-auto.sh
	rm -f choose
	cat warn-auto.sh choose.sh \
	| sed s}HOME}"`head -1 home`"}g \
	> choose
	chmod 555 choose

coe.o: coe.c coe.h compile
	./compile coe.c

compile: conf-cc print-cc.sh systype warn-auto.sh
	rm -f compile
	sh print-cc.sh > compile
	chmod 555 compile

deepsleep.o: compile deepsleep.c deepsleep.h iopause.h tai.h taia.h \
uint64.h
	./compile deepsleep.c

direntry.h: choose compile direntry.h1 direntry.h2 trydrent.c
	./choose c trydrent direntry.h1 direntry.h2 > direntry.h

env.o: compile env.c env.h str.h
	./compile env.c

envdir: byte.a envdir.o load unix.a
	./load envdir unix.a byte.a 

envdir.o: byte.h compile direntry.h envdir.c error.h gen_alloc.h \
open.h openreadclose.h pathexec.h stralloc.h strerr.h
	./compile envdir.c

envuidgid: byte.a envuidgid.o load unix.a
	./load envuidgid unix.a byte.a 

envuidgid.o: compile envuidgid.c fmt.h pathexec.h strerr.h
	./compile envuidgid.c

error.o: compile error.c error.h
	./compile error.c

error_str.o: compile error.h error_str.c
	./compile error_str.c

fd_copy.o: compile fd.h fd_copy.c
	./compile fd_copy.c

fd_move.o: compile fd.h fd_move.c
	./compile fd_move.c

fghack: byte.a fghack.o load unix.a
	./load fghack unix.a byte.a 

fghack.o: buffer.h compile error.h fghack.c pathexec.h strerr.h \
wait.h
	./compile fghack.c

fifo.o: compile fifo.c fifo.h hasmkffo.h
	./compile fifo.c

fmt_uint.o: compile fmt.h fmt_uint.c
	./compile fmt_uint.c

fmt_uint0.o: compile fmt.h fmt_uint0.c
	./compile fmt_uint0.c

fmt_ulong.o: compile fmt.h fmt_ulong.c
	./compile fmt_ulong.c

hasflock.h: choose compile hasflock.h1 hasflock.h2 load tryflock.c
	./choose cl tryflock hasflock.h1 hasflock.h2 > hasflock.h

hasmkffo.h: choose compile hasmkffo.h1 hasmkffo.h2 load trymkffo.c
	./choose cl trymkffo hasmkffo.h1 hasmkffo.h2 > hasmkffo.h

hassgact.h: choose compile hassgact.h1 hassgact.h2 load trysgact.c
	./choose cl trysgact hassgact.h1 hassgact.h2 > hassgact.h

hassgprm.h: choose compile hassgprm.h1 hassgprm.h2 load trysgprm.c
	./choose cl trysgprm hassgprm.h1 hassgprm.h2 > hassgprm.h

hasshsgr.h: chkshsgr choose compile hasshsgr.h1 hasshsgr.h2 load \
tryshsgr.c warn-shsgr
	./chkshsgr || ( cat warn-shsgr; exit 1 )
	./choose clr tryshsgr hasshsgr.h1 hasshsgr.h2 > hasshsgr.h

haswaitp.h: choose compile haswaitp.h1 haswaitp.h2 load trywaitp.c
	./choose cl trywaitp haswaitp.h1 haswaitp.h2 > haswaitp.h

iopause.h: choose compile iopause.h1 iopause.h2 load trypoll.c
	./choose clr trypoll iopause.h1 iopause.h2 > iopause.h

iopause.o: compile iopause.c iopause.h select.h tai.h taia.h uint64.h
	./compile iopause.c

it: rts sysdeps

load: conf-ld print-ld.sh systype warn-auto.sh
	rm -f load
	sh print-ld.sh > load
	chmod 555 load

lock_ex.o: compile hasflock.h lock.h lock_ex.c
	./compile lock_ex.c

lock_exnb.o: compile hasflock.h lock.h lock_exnb.c
	./compile lock_exnb.c

makelib: print-ar.sh systype warn-auto.sh
	rm -f makelib
	sh print-ar.sh > makelib
	chmod 555 makelib

match.o: compile match.c match.h
	./compile match.c

matchtest: byte.a load match.o matchtest.o unix.a
	./load matchtest match.o unix.a byte.a 

matchtest.o: buffer.h compile match.h matchtest.c str.h
	./compile matchtest.c

multilog: byte.a deepsleep.o load match.o multilog.o time.a \
timestamp.o unix.a
	./load multilog deepsleep.o timestamp.o match.o time.a unix.a byte.a 

multilog.o: alloc.h buffer.h byte.h coe.h compile deepsleep.h \
direntry.h env.h error.h fd.h lock.h match.h multilog.c open.h scan.h \
seek.h sig.h str.h strerr.h timestamp.h wait.h
	./compile multilog.c

ndelay_off.o: compile ndelay.h ndelay_off.c
	./compile ndelay_off.c

ndelay_on.o: compile ndelay.h ndelay_on.c
	./compile ndelay_on.c

open_append.o: compile open.h open_append.c
	./compile open_append.c

open_read.o: compile open.h open_read.c
	./compile open_read.c

open_trunc.o: compile open.h open_trunc.c
	./compile open_trunc.c

open_write.o: compile open.h open_write.c
	./compile open_write.c

openreadclose.o: compile error.h gen_alloc.h open.h openreadclose.c \
openreadclose.h readclose.h stralloc.h
	./compile openreadclose.c

pathexec_env.o: alloc.h byte.h compile env.h gen_alloc.h pathexec.h \
pathexec_env.c str.h stralloc.h
	./compile pathexec_env.c

pathexec_run.o: compile env.h error.h gen_alloc.h pathexec.h \
pathexec_run.c str.h stralloc.h
	./compile pathexec_run.c

pgrphack: byte.a load pgrphack.o unix.a
	./load pgrphack unix.a byte.a 

pgrphack.o: compile pathexec.h pgrphack.c strerr.h
	./compile pgrphack.c

prot.o: compile hasshsgr.h prot.c prot.h
	./compile prot.c

readclose.o: compile error.h gen_alloc.h readclose.c readclose.h \
stralloc.h
	./compile readclose.c

readproctitle: byte.a load readproctitle.o unix.a
	./load readproctitle unix.a byte.a 

readproctitle.o: compile error.h readproctitle.c
	./compile readproctitle.c

rts: envdir envuidgid fghack matchtest multilog pgrphack \
readproctitle rts.tests setlock setuidgid softlimit supervise svc \
svok svscan svscanboot svstat tai64n tai64nlocal
	env - /bin/sh rts.tests 2>&1 | cat -v > rts

scan_ulong.o: compile scan.h scan_ulong.c
	./compile scan_ulong.c

seek_set.o: compile seek.h seek_set.c
	./compile seek_set.c

select.h: choose compile select.h1 select.h2 trysysel.c
	./choose c trysysel select.h1 select.h2 > select.h

setlock: byte.a load setlock.o unix.a
	./load setlock unix.a byte.a 

setlock.o: compile lock.h open.h pathexec.h setlock.c sgetopt.h \
strerr.h subgetopt.h
	./compile setlock.c

setuidgid: byte.a load setuidgid.o unix.a
	./load setuidgid unix.a byte.a 

setuidgid.o: compile pathexec.h prot.h setuidgid.c strerr.h
	./compile setuidgid.c

sgetopt.o: buffer.h compile sgetopt.c sgetopt.h subgetopt.h
	./compile sgetopt.c

sig.o: compile sig.c sig.h
	./compile sig.c

sig_block.o: compile hassgprm.h sig.h sig_block.c
	./compile sig_block.c

sig_catch.o: compile hassgact.h sig.h sig_catch.c
	./compile sig_catch.c

sig_pause.o: compile hassgprm.h sig.h sig_pause.c
	./compile sig_pause.c

softlimit: byte.a load softlimit.o unix.a
	./load softlimit unix.a byte.a 

softlimit.o: compile pathexec.h scan.h sgetopt.h softlimit.c str.h \
strerr.h subgetopt.h
	./compile softlimit.c

str_chr.o: compile str.h str_chr.c
	./compile str_chr.c

str_diff.o: compile str.h str_diff.c
	./compile str_diff.c

str_len.o: compile str.h str_len.c
	./compile str_len.c

str_start.o: compile str.h str_start.c
	./compile str_start.c

stralloc_cat.o: byte.h compile gen_alloc.h stralloc.h stralloc_cat.c
	./compile stralloc_cat.c

stralloc_catb.o: byte.h compile gen_alloc.h stralloc.h \
stralloc_catb.c
	./compile stralloc_catb.c

stralloc_cats.o: byte.h compile gen_alloc.h str.h stralloc.h \
stralloc_cats.c
	./compile stralloc_cats.c

stralloc_eady.o: alloc.h compile gen_alloc.h gen_allocdefs.h \
stralloc.h stralloc_eady.c
	./compile stralloc_eady.c

stralloc_opyb.o: byte.h compile gen_alloc.h stralloc.h \
stralloc_opyb.c
	./compile stralloc_opyb.c

stralloc_opys.o: byte.h compile gen_alloc.h str.h stralloc.h \
stralloc_opys.c
	./compile stralloc_opys.c

stralloc_pend.o: alloc.h compile gen_alloc.h gen_allocdefs.h \
stralloc.h stralloc_pend.c
	./compile stralloc_pend.c

strerr_die.o: buffer.h compile strerr.h strerr_die.c
	./compile strerr_die.c

strerr_sys.o: compile error.h strerr.h strerr_sys.c
	./compile strerr_sys.c

subgetopt.o: compile subgetopt.c subgetopt.h
	./compile subgetopt.c

supervise: byte.a deepsleep.o load supervise.o time.a unix.a
	./load supervise deepsleep.o time.a unix.a byte.a 

supervise.o: coe.h compile deepsleep.h env.h error.h fifo.h iopause.h \
lock.h ndelay.h open.h sig.h strerr.h supervise.c tai.h taia.h \
uint64.h wait.h
	./compile supervise.c

svc: byte.a load svc.o unix.a
	./load svc unix.a byte.a 

svc.o: buffer.h byte.h compile error.h ndelay.h open.h sgetopt.h \
sig.h strerr.h subgetopt.h svc.c
	./compile svc.c

svok: byte.a load svok.o unix.a
	./load svok unix.a byte.a 

svok.o: compile error.h open.h strerr.h svok.c
	./compile svok.c

svscan: byte.a load svscan.o unix.a
	./load svscan unix.a byte.a 

svscan.o: byte.h coe.h compile direntry.h env.h error.h fd.h \
pathexec.h str.h strerr.h svscan.c wait.h
	./compile svscan.c

svscanboot: home svscanboot.sh warn-auto.sh
	rm -f svscanboot
	cat warn-auto.sh svscanboot.sh \
	| sed s}HOME}"`head -1 home`"}g \
	> svscanboot
	chmod 555 svscanboot

svstat: byte.a load svstat.o time.a unix.a
	./load svstat time.a unix.a byte.a 

svstat.o: buffer.h compile error.h fmt.h open.h strerr.h svstat.c \
tai.h uint64.h
	./compile svstat.c

sysdeps: compile direntry.h hasflock.h hasmkffo.h hassgact.h \
hassgprm.h hasshsgr.h haswaitp.h iopause.h load select.h systype \
uint64.h
	rm -f sysdeps
	cat systype compile load >> sysdeps
	grep sysdep direntry.h >> sysdeps
	grep sysdep haswaitp.h >> sysdeps
	grep sysdep hassgact.h >> sysdeps
	grep sysdep hassgprm.h >> sysdeps
	grep sysdep select.h >> sysdeps
	grep sysdep uint64.h >> sysdeps
	grep sysdep iopause.h >> sysdeps
	grep sysdep hasmkffo.h >> sysdeps
	grep sysdep hasflock.h >> sysdeps
	grep sysdep hasshsgr.h >> sysdeps

systype: find-systype.sh trycpp.c x86cpuid.c
	sh find-systype.sh > systype

tai64n: byte.a load tai64n.o time.a timestamp.o unix.a
	./load tai64n timestamp.o time.a unix.a byte.a 

tai64n.o: buffer.h compile tai64n.c timestamp.h
	./compile tai64n.c

tai64nlocal: byte.a load tai64nlocal.o unix.a
	./load tai64nlocal unix.a byte.a 

tai64nlocal.o: buffer.h compile fmt.h tai64nlocal.c
	./compile tai64nlocal.c

tai_now.o: compile tai.h tai_now.c uint64.h
	./compile tai_now.c

tai_pack.o: compile tai.h tai_pack.c uint64.h
	./compile tai_pack.c

tai_sub.o: compile tai.h tai_sub.c uint64.h
	./compile tai_sub.c

tai_unpack.o: compile tai.h tai_unpack.c uint64.h
	./compile tai_unpack.c

taia_add.o: compile tai.h taia.h taia_add.c uint64.h
	./compile taia_add.c

taia_approx.o: compile tai.h taia.h taia_approx.c uint64.h
	./compile taia_approx.c

taia_frac.o: compile tai.h taia.h taia_frac.c uint64.h
	./compile taia_frac.c

taia_less.o: compile tai.h taia.h taia_less.c uint64.h
	./compile taia_less.c

taia_now.o: compile tai.h taia.h taia_now.c uint64.h
	./compile taia_now.c

taia_pack.o: compile tai.h taia.h taia_pack.c uint64.h
	./compile taia_pack.c

taia_sub.o: compile tai.h taia.h taia_sub.c uint64.h
	./compile taia_sub.c

taia_uint.o: compile tai.h taia.h taia_uint.c uint64.h
	./compile taia_uint.c

time.a: iopause.o makelib tai_now.o tai_pack.o tai_sub.o tai_unpack.o \
taia_add.o taia_approx.o taia_frac.o taia_less.o taia_now.o \
taia_pack.o taia_sub.o taia_uint.o
	./makelib time.a iopause.o tai_now.o tai_pack.o tai_sub.o \
	tai_unpack.o taia_add.o taia_approx.o taia_frac.o taia_less.o \
	taia_now.o taia_pack.o taia_sub.o taia_uint.o

timestamp.o: compile tai.h taia.h timestamp.c timestamp.h uint64.h
	./compile timestamp.c

uint64.h: choose compile load tryulong64.c uint64.h1 uint64.h2
	./choose clr tryulong64 uint64.h1 uint64.h2 > uint64.h

unix.a: alloc.o alloc_re.o buffer.o buffer_0.o buffer_1.o buffer_2.o \
buffer_get.o buffer_put.o buffer_read.o buffer_write.o coe.o env.o \
error.o error_str.o fd_copy.o fd_move.o fifo.o lock_ex.o lock_exnb.o \
makelib ndelay_off.o ndelay_on.o open_append.o open_read.o \
open_trunc.o open_write.o openreadclose.o pathexec_env.o \
pathexec_run.o prot.o readclose.o seek_set.o sgetopt.o sig.o \
sig_block.o sig_catch.o sig_pause.o stralloc_cat.o stralloc_catb.o \
stralloc_cats.o stralloc_eady.o stralloc_opyb.o stralloc_opys.o \
stralloc_pend.o strerr_die.o strerr_sys.o subgetopt.o wait_nohang.o \
wait_pid.o
	./makelib unix.a alloc.o alloc_re.o buffer.o buffer_0.o buffer_1.o \
	buffer_2.o buffer_get.o buffer_put.o buffer_read.o buffer_write.o \
	coe.o env.o error.o error_str.o fd_copy.o fd_move.o fifo.o lock_ex.o \
	lock_exnb.o ndelay_off.o ndelay_on.o open_append.o open_read.o \
	open_trunc.o open_write.o openreadclose.o pathexec_env.o \
	pathexec_run.o prot.o readclose.o seek_set.o sgetopt.o sig.o \
	sig_block.o sig_catch.o sig_pause.o stralloc_cat.o stralloc_catb.o \
	stralloc_cats.o stralloc_eady.o stralloc_opyb.o stralloc_opys.o \
	stralloc_pend.o strerr_die.o strerr_sys.o subgetopt.o wait_nohang.o \
	wait_pid.o

wait_nohang.o: compile haswaitp.h wait_nohang.c
	./compile wait_nohang.c

wait_pid.o: compile error.h haswaitp.h wait_pid.c
	./compile wait_pid.c

