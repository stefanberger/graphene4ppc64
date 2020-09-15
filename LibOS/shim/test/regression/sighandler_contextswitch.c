#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>

#define stringize(x)		#x
#define stringify(x)		stringize(x)

#define FR_REG                  19
#define FR_MAIN			3.141592
#define FR_HANDLER		2.71828

#define VSR_REG			17 /* this one must be < 32 */
#define VSR_MAIN_LO		0xcafecafecafecafeULL
#define VSR_MAIN_HI		0xbadebadebadebadeULL
#define VSR_HANDLER_LO		0x1122334455667788ULL
#define VSR_HANDLER_HI		0x99aabbccddeeff00ULL

#define VSR2_REG		35 /* this one must be >= 32 */
#define VSR2_MAIN_LO		0x1122334455667788ULL
#define VSR2_MAIN_HI		0x99aabbccddeeff00ULL
#define VSR2_HANDLER_LO		0xcafecafecafecafeULL
#define VSR2_HANDLER_HI		0xbadebadebadebadeULL

#define VRSAVE_MAIN		0x12345678UL
#define VRSAVE_HANDLER		0x87654321UL

#define VSCR_MAIN		0x00010000UL
#define VSCR_HANDLER		0x00010001UL

static unsigned int good;

#define GOOD_FP			(1 << 0)
#define GOOD_VSR		(1 << 1)
#define GOOD_VSR2		(1 << 2)
#define GOOD_VRSAVE		(1 << 3)
#define GOOD_VSCR		(1 << 4)

static void __attribute__((noinline)) set_vsr(unsigned long lo, unsigned long hi) {
    unsigned long data[2] = { lo, hi };

    __asm__ __volatile__ (
        "mr 11, %0\n\t"
        "lxvd2x " stringify(VSR_REG) ", 0, 11\n\t"
        :
        : "r" (&data)
        : "r11", stringify(VSR_REG)
    );
}

static void __attribute__((noinline)) get_vsr(unsigned long *lo, unsigned long *hi) {
    unsigned long data[2];

    __asm__ __volatile__ (
        "mr 11, %0\n\t"
        "stxvd2x " stringify(VSR_REG) ", 0, 11\n\t"
        :
        : "r" (&data)
        : "r11", "memory"
    );
    *lo = data[0];
    *hi = data[1];
}

static void __attribute__((noinline)) set_vsr2(unsigned long lo, unsigned long hi) {
    unsigned long data[2] = { lo, hi };

    __asm__ __volatile__ (
        "mr 11, %0\n\t"
        "lxvd2x " stringify(VSR2_REG) ", 0, 11\n\t"
        :
        : "r" (&data)
        : "r11", stringify(VSR2_REG)
    );
}

static void __attribute__((noinline)) get_vsr2(unsigned long *lo, unsigned long *hi) {
    unsigned long data[2];

    __asm__ __volatile__ (
        "mr 11, %0\n\t"
        "stxvd2x " stringify(VSR2_REG) ", 0, 11\n\t"
        :
        : "r" (&data)
        : "r11", "memory"
    );
    *lo = data[0];
    *hi = data[1];
}

static void __attribute__((noinline)) set_vrsave(unsigned long vrsave) {
    __asm__ __volatile__ (
        "mtvrsave %0\n\t"
        :
        : "r" (vrsave)
        : "vrsave"
    );
}

static unsigned long __attribute__((noinline)) get_vrsave(void) {
    unsigned long vrsave;

    __asm__ __volatile__ (
        "mfvrsave %0\n\t"
        : "=r" (vrsave)
        :
        :
    );
    return vrsave;
}
static void __attribute__((noinline)) set_vscr(unsigned long vscr) {
    unsigned long data[2] = { 0, vscr };

    __asm__ __volatile__ (
        "mr 11, %0\n\t"
        "lxvd2x 32, 0, 11\n\t"
        "mtvscr 0\n\t"
        :
        : "r" (&data)
        : "v0", "vs32", "r11", "vscr"
    );
}
static unsigned long __attribute__((noinline)) get_vscr(void) {
    unsigned long data[2];

    __asm__ __volatile__ (
        "mfvscr 0\n\t"
        "mr 11, %0\n\t"
        "stxvd2x 32, 0, 11\n\t"
        :
        : "r" (&data)
        : "v0", "vs32", "r11", "memory"
    );
    return data[1];
}

struct local_vscr
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    unsigned int __pad[3];
    unsigned int vscr_word;
#else
    unsigned int vscr_word;
    unsigned int __pad[3];
#endif
};

static void handler(int signum, siginfo_t* si, void* puc) {
    ucontext_t* uc = puc;
    mcontext_t* mc = &uc->uc_mcontext;

    double* fpnum = (double*)&mc->__fp_regs[FR_REG];
    if (*fpnum == FR_MAIN)
        good |= GOOD_FP;
    *fpnum = FR_HANDLER;

    vrregset_t *v_regs = mc->__v_regs;
    unsigned long vsr_lo = *(unsigned long*)&mc->__fp_regs[VSR_REG];
    double *vsx_hi_regs =(void *)((unsigned long)v_regs + __NVRREG * 16);
    unsigned long vsr_hi = *(unsigned long*)&vsx_hi_regs[VSR_REG];

    if (vsr_lo == VSR_MAIN_LO && vsr_hi == VSR_MAIN_HI)
        good |= GOOD_VSR;

    *(unsigned long *)&uc->uc_mcontext.__fp_regs[VSR_REG] = VSR_HANDLER_LO;
    *(unsigned long *)&vsx_hi_regs[VSR_REG] = VSR_HANDLER_HI;

    unsigned long vsr2_hi = *(unsigned long *)&(v_regs->__vrregs[VSR2_REG - 32][0]);
    unsigned long vsr2_lo = *(unsigned long *)&(v_regs->__vrregs[VSR2_REG - 32][2]);

    if (vsr2_lo == VSR2_MAIN_LO && vsr2_hi == VSR2_MAIN_HI)
        good |= GOOD_VSR2;

    *(unsigned long *)&(v_regs->__vrregs[VSR2_REG - 32][0]) = VSR2_HANDLER_HI;
    *(unsigned long *)&(v_regs->__vrregs[VSR2_REG - 32][2]) = VSR2_HANDLER_LO;

    unsigned long vrsave = v_regs->__vrsave;
    if (vrsave == VRSAVE_MAIN)
        good |= GOOD_VRSAVE;
    v_regs->__vrsave = VRSAVE_HANDLER;

    struct local_vscr *lvscr = (struct local_vscr *)(&v_regs->__vscr);
    unsigned int vscr = lvscr->vscr_word;

    if (vscr == VSCR_MAIN)
        good |= GOOD_VSCR;
    else
        printf("in handler: vscr = 0x%x expected = 0x%lx\n", vscr, VSCR_MAIN);
    lvscr->vscr_word = VSCR_HANDLER;
}

int main() {
    struct sigaction action = {
        .sa_sigaction = handler,
        .sa_flags = SA_SIGINFO,
    };
    int ret = sigaction(SIGINT, &action, NULL);
    if (ret < 0) {
        fprintf(stderr, "sigaction failed\n");
        return 1;
    }

    register double fpnum __asm__("fr" stringify(FR_REG)) = FR_MAIN;
    set_vsr(VSR_MAIN_LO, VSR_MAIN_HI);
    set_vsr2(VSR2_MAIN_LO, VSR2_MAIN_HI);
    set_vrsave(VRSAVE_MAIN);
    set_vscr(VSCR_MAIN);
    //printf("vscr before: 0x%lx\n", get_vscr());

    kill(getpid(), SIGINT);
    if (!(good & GOOD_FP)) {
        printf("'Float' did not show proper value in signal handler\n");
        ret = 1;
    }

    if (fpnum != FR_HANDLER) {
        printf("Context switch failed: Wrong double value from handler\n");
        ret = 1;
    }

    unsigned long vsr_lo, vsr_hi;
    get_vsr(&vsr_lo, &vsr_hi);
    if (!(good & GOOD_VSR)) {
        printf("'VSR%u' did not show proper value in signal handler\n", VSR_REG);
        ret = 1;
    }
    if (vsr_lo != VSR_HANDLER_LO || vsr_hi != VSR_HANDLER_HI) {
        printf("Context switch failed: VSR%u has wrong value from handler\n", VSR_REG);
        ret = 1;
    }

    unsigned long vsr2_lo, vsr2_hi;
    get_vsr2(&vsr2_lo, &vsr2_hi);
    if (!(good & GOOD_VSR2)) {
        printf("'VSR%u' did not show proper value in signal handler\n", VSR2_REG);
        ret = 1;
    }
    if (vsr2_lo != VSR2_HANDLER_LO || vsr2_hi != VSR2_HANDLER_HI) {
        printf("Context switch failed: VSR%u has wrong value from handler\n", VSR2_REG);
        printf("vsr2_lo: 0x%lx [0x%llx]  vsr2_hi: 0x%lx [0x%llx]\n",
               vsr2_lo, VSR2_HANDLER_LO, vsr2_hi, VSR2_HANDLER_HI);
        ret = 1;
    }

    unsigned long vrsave = get_vrsave();
    if (!(good & GOOD_VRSAVE)) {
        printf("'VRSAVE' did not show proper value in signal handler\n");
        ret = 1;
    }
    if (vrsave != VRSAVE_HANDLER) {
       printf("Context switch failed: VRSAVE has wrong value from handler\n");
       ret = 1;
    }

    //printf("vscr: 0x%lx  vrsave: 0x%lx\n", get_vscr(), get_vrsave());
    unsigned long vscr = get_vscr();
    vscr = get_vscr();
    if (!(good & GOOD_VSCR)) {
        printf("'VSCR' did not show proper value in signal handler\n");
        ret = 1;
    }
    if (vscr != VSCR_HANDLER) {
       printf("Context switch failed: VSCR has wrong value from handler\n");
       printf("vscr: 0x%lx  vrsave: 0x%lx\n", vscr, get_vrsave());
       ret = 1;
    }
    if (!ret)
        printf("TEST OK\n");

    return ret;
}
