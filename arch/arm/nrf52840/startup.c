#define STACK_TOP (void *)0x20005000

extern unsigned long _stext;
extern unsigned long _sbss;
extern unsigned long _sdata;
extern unsigned long _etext;
extern unsigned long _ebss;
extern unsigned long _edata;

void c_startup(void);
void dummy_fn(void);

int main();

__attribute__((section(".isr_vector")))
void (*vectors[])(void) = {
        STACK_TOP,
        c_startup,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
};

void dummy_fn(void)
{
        while(1)
        {
                
        }
}

void c_startup(void)
{
        unsigned long *src, *dst;
        
        src = &_etext;
        dst = &_sdata;
        while(dst < &_edata) 
                *(dst++) = *(src++);
        
        src = &_sbss;
        while(src < &_ebss) 
                *(src++) = 0;

        main();
}
