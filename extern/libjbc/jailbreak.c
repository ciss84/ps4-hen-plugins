#include "defs.h"
#include "kernelrw.h"
#include "jailbreak.h"

asm("open:\nmov $5, %rax\nmov %rcx, %r10\nsyscall\nret");
asm("close:\nmov $6, %rax\nmov %rcx, %r10\nsyscall\nret");

int open(const char*, int);
int close(int);

static uintptr_t prison0;
static uintptr_t rootvnode;

static int resolve(void)
{
restart:;
    uintptr_t td = jbc_krw_get_td();
    uintptr_t proc = jbc_krw_read64(td+8, KERNEL_HEAP);
    for(;;)
    {
        int pid;
        if(jbc_krw_memcpy((uintptr_t)&pid, proc+0xb0, sizeof(pid), KERNEL_HEAP))
            goto restart;
        if(pid == 1)
            break;
        uintptr_t proc2 = jbc_krw_read64(proc, KERNEL_HEAP);
        uintptr_t proc1 = jbc_krw_read64(proc2+8, KERNEL_HEAP);
        if(proc1 != proc)
            goto restart;
        proc = proc2;
    }
    uintptr_t pid1_ucred = jbc_krw_read64(proc+0x40, KERNEL_HEAP);
    uintptr_t pid1_fd = jbc_krw_read64(proc+0x48, KERNEL_HEAP);
    if(jbc_krw_memcpy((uintptr_t)&prison0, pid1_ucred+0x30, sizeof(prison0), KERNEL_HEAP))
        return -1;
    if(jbc_krw_memcpy((uintptr_t)&rootvnode, pid1_fd+0x18, sizeof(rootvnode), KERNEL_HEAP))
    {
        prison0 = 0;
        return -1;
    }
    return 0;
}

uintptr_t jbc_get_prison0(void)
{
    if(!prison0)
        resolve();
    return prison0;
}

uintptr_t jbc_get_rootvnode(void)
{
    if(!rootvnode)
        resolve();
    return rootvnode;
}

static inline int ppcopyout(void* u1, void* u2, uintptr_t k)
{
    return jbc_krw_memcpy((uintptr_t)u1, k, (uintptr_t)u2-(uintptr_t)u1, KERNEL_HEAP);
}

static inline int ppcopyin(const void* u1, const void* u2, uintptr_t k)
{
    return jbc_krw_memcpy(k, (uintptr_t)u1, (uintptr_t)u2-(uintptr_t)u1, KERNEL_HEAP);
}

int jbc_get_cred(struct jbc_cred* ans)
{
    uintptr_t td = jbc_krw_get_td();
    uintptr_t proc = jbc_krw_read64(td + 8, KERNEL_HEAP);
    uintptr_t ucred = jbc_krw_read64(proc + 0x40, KERNEL_HEAP);
    uintptr_t fd = jbc_krw_read64(proc + 0x48, KERNEL_HEAP);

    if (ppcopyout(&ans->uid, 1 + &ans->svuid, ucred + 4)
        || ppcopyout(&ans->rgid, 1 + &ans->svgid, ucred + 20)
        || ppcopyout(&ans->prison, 1 + &ans->prison, ucred + 0x30)
        || ppcopyout(&ans->cdir, 1 + &ans->jdir, fd + 0x10)
        || ppcopyout(&ans->sceProcType, 1 + &ans->sceProcCap, ucred + 88))
        return -1;

    return 0;
}

static int jbc_set_cred_internal(const struct jbc_cred* ans)
{
    uintptr_t td = jbc_krw_get_td();
    uintptr_t proc = jbc_krw_read64(td + 8, KERNEL_HEAP);
    uintptr_t ucred = jbc_krw_read64(proc + 0x40, KERNEL_HEAP);
    uintptr_t fd = jbc_krw_read64(proc + 0x48, KERNEL_HEAP);

    if (ppcopyin(&ans->uid, 1 + &ans->svuid, ucred + 4)
        || ppcopyin(&ans->rgid, 1 + &ans->svgid, ucred + 20)
        || ppcopyin(&ans->prison, 1 + &ans->prison, ucred + 0x30)
        || ppcopyin(&ans->cdir, 1 + &ans->jdir, fd + 0x10) 
        || ppcopyin(&ans->sceProcType, 1 + &ans->sceProcCap, ucred + 88))
        return -1;
    return 0;
}

int jbc_jailbreak_cred(struct jbc_cred* ans)
{
    uintptr_t prison0 = jbc_get_prison0();
    if (!prison0)
        return -1;
    uintptr_t rootvnode = jbc_get_rootvnode();
    if (!rootvnode)
        return -1;

    //without some modules wont load like Apputils
    ans->sceProcCap = 0xffffffffffffffff;
    ans->sceProcType = 0x3801000000000013;
    ans->sonyCred = 0xffffffffffffffff;

    ans->uid = ans->ruid = ans->svuid = ans->rgid = ans->svgid = 0;
    ans->prison = prison0;
    ans->cdir = ans->rdir = ans->jdir = rootvnode;
    return 0;
}

static int jbc_jailbreak_cred_device(struct jbc_cred* ans)
{
    const uintptr_t rootvnode = jbc_get_rootvnode();
    if (!rootvnode)
    {
        return -1;
    }
    // for devices
    ans->sceProcCap = 0xffffffffffffffff;
    ans->uid = 0;
    ans->rdir = rootvnode;
    return 0;
}

void open_dev_console(void (*do_dup)(int))
{
    // user must provide handler
    if (!do_dup)
    {
        return;
    }
    struct jbc_cred g_Cred;
    struct jbc_cred g_RootCreds;
    jbc_get_cred(&g_Cred);
    g_RootCreds = g_Cred;
    jbc_jailbreak_cred_device(&g_RootCreds);
    jbc_set_cred(&g_RootCreds);
    const int O_WRONLY_ = 2;
    const int fd = open("/dev/console", O_WRONLY_);
    if (fd > 0)
    {
        do_dup(fd);
        close(fd);
    }
    // unjailbreak
    jbc_set_cred(&g_Cred);
}

static int jbc_open_this(const struct jbc_cred* cred0, uintptr_t vnode)
{
    if(!vnode)
        return -1;
    struct jbc_cred cred = *cred0;
    jbc_jailbreak_cred(&cred);
    cred.cdir = cred.rdir = cred.jdir = vnode;
    jbc_set_cred_internal(&cred);
    int ans = open("/", 0);
    jbc_set_cred_internal(cred0);
    return ans;
}

static int swap64(uintptr_t p1, KmemKind k1, uintptr_t p2, KmemKind k2)
{
    uintptr_t v1 = jbc_krw_read64(p1, k1);
    uintptr_t v2 = jbc_krw_read64(p2, k2);
    if(ppcopyin(&v2, 1+&v2, p1)
    || ppcopyin(&v1, 1+&v1, p2))
        return -1;
    return 0;
}

static int return0(void)
{
    return 0;
}

static void* fake_vtable[16] = {};

int jbc_set_cred(const struct jbc_cred* newp)
{
    struct jbc_cred old, neww = *newp;
    if(jbc_get_cred(&old))
        return -1;
    if(old.cdir && !neww.cdir)
        neww.cdir = jbc_get_rootvnode();
    if(old.rdir && !neww.rdir)
        neww.rdir = jbc_get_rootvnode();
    if(old.jdir && !neww.jdir)
        neww.jdir = jbc_get_rootvnode();
    int cdir_fd = jbc_open_this(&old, neww.cdir);
    int rdir_fd = jbc_open_this(&old, neww.rdir);
    int jdir_fd = jbc_open_this(&old, neww.jdir);
    struct jbc_cred elevated = neww;
    elevated.cdir = old.cdir;
    elevated.jdir = old.jdir;
    elevated.rdir = old.rdir;
    //this offset is the same in 5.05-9.60, probably safe to use
    uint32_t rc;
    if(elevated.prison)
    {
        if(jbc_krw_memcpy((uintptr_t)&rc, elevated.prison+0x14, sizeof(rc), KERNEL_HEAP)
        && jbc_krw_memcpy((uintptr_t)&rc, elevated.prison+0x14, sizeof(rc), KERNEL_TEXT))
            return -1;
        rc++;
        if(jbc_krw_memcpy(elevated.prison+0x14, (uintptr_t)&rc, sizeof(rc), KERNEL_HEAP)
        && jbc_krw_memcpy(elevated.prison+0x14, (uintptr_t)&rc, sizeof(rc), KERNEL_TEXT))
            return -1;
    }
    if(jbc_set_cred_internal(&elevated))
        return -1;
    if(old.prison)
    {
        if(jbc_krw_memcpy((uintptr_t)&rc, old.prison+0x14, sizeof(rc), KERNEL_HEAP)
        && jbc_krw_memcpy((uintptr_t)&rc, old.prison+0x14, sizeof(rc), KERNEL_TEXT))
            return -1;
        rc--;
        if(jbc_krw_memcpy(old.prison+0x14, (uintptr_t)&rc, sizeof(rc), KERNEL_HEAP)
        && jbc_krw_memcpy(old.prison+0x14, (uintptr_t)&rc, sizeof(rc), KERNEL_TEXT))
            return -1;
    }
    uintptr_t td = jbc_krw_get_td();
    uintptr_t proc = jbc_krw_read64(td + 8, KERNEL_HEAP);
    // uintptr_t ucred = jbc_krw_read64(proc + 0x40, KERNEL_HEAP); // unused
    uintptr_t fd = jbc_krw_read64(proc + 0x48, KERNEL_HEAP);
    uintptr_t ofiles = jbc_krw_read64(fd, KERNEL_HEAP);
    int fds[3] = {cdir_fd, rdir_fd, jdir_fd};
    for(int i = 0; i < 3; i++)
    {
        if(fds[i] < 0)
            continue;
        uintptr_t file = jbc_krw_read64(ofiles+8*fds[i], KERNEL_HEAP);
        if(swap64(file, KERNEL_HEAP, fd+0x10+8*i, KERNEL_HEAP))
            return -1;
        if(jbc_krw_read64(file, KERNEL_HEAP) == 0)
        {
            if(!fake_vtable[0])
                for(int i = 0; i < sizeof(fake_vtable) / sizeof(fake_vtable[0]); i++)
                    fake_vtable[i] = return0;
            uintptr_t p = (uintptr_t)fake_vtable;
            if(ppcopyin(&p, 1+&p, file+8))
                return -1;
        }
        close(fds[i]);
    }
    return 0;
}
