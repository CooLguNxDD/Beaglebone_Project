
#include <linux/module.h>
#include <linux/miscdevice.h>		// for misc-driver calls.
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/uaccess.h>

//#error Are we building this?

#define MY_DEVICE_FILE  "morse-code"

/**************************************************************
 * FIFO Support
 *************************************************************/

#include <linux/kfifo.h>
#define FIFO_SIZE 512	// Must be a power of 2.
static int led_state_t = 0;
static DECLARE_KFIFO(echo_fifo, char, FIFO_SIZE);

/******************************************************
 * LED
 ******************************************************/
#include <linux/leds.h>

DEFINE_LED_TRIGGER(ledtrig_demo);


static void my_led_on(void){
    // led off
    if(led_state_t == 0){
        led_state_t = 1;
        led_trigger_event(ledtrig_demo, LED_FULL);
    }
}
static void my_led_off(void){
    // led on
    if(led_state_t == 1){
        led_state_t = 0;
        led_trigger_event(ledtrig_demo, LED_OFF);
    }
}
static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple("morse-code", &ledtrig_demo);
}
static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(ledtrig_demo);
}
/******************************************************
 * morse function
 ******************************************************/
// Morse Code Encodings (from http://en.wikipedia.org/wiki/Morse_code)
//   Encoding created by Brian Fraser. Released under GPL.
//
// Encoding description:
// - msb to be output first, followed by 2nd msb... (left to right)
// - each bit gets one "dot" time.
// - "dashes" are encoded here as being 3 times as long as "dots". Therefore
//   a single dash will be the bits: 111.
// - ignore trailing 0's (once last 1 output, rest of 0's ignored).
// - Space between dashes and dots is one dot time, so is therefore encoded
//   as a 0 bit between two 1 bits.
//
// Example:
//   R = dot   dash   dot       -- Morse code
//     =  1  0 111  0  1        -- 1=LED on, 0=LED off
//     =  1011 101              -- Written together in groups of 4 bits.
//     =  1011 1010 0000 0000   -- Pad with 0's on right to make 16 bits long.
//     =  B    A    0    0      -- Convert to hex digits
//     = 0xBA00                 -- Full hex value (see value in table below)
//
// Between characters, must have 3-dot times (total) of off (0's) (not encoded here)
// Between words, must have 7-dot times (total) of off (0's) (not encoded here).
//

#define DOT_TIME 200
static unsigned short morsecode_codes[] = {
        0xB800,	// A 1011 1
        0xEA80,	// B 1110 1010 1
        0xEBA0,	// C 1110 1011 101
        0xEA00,	// D 1110 101
        0x8000,	// E 1
        0xAE80,	// F 1010 1110 1
        0xEE80,	// G 1110 1110 1
        0xAA00,	// H 1010 101
        0xA000,	// I 101
        0xBBB8,	// J 1011 1011 1011 1
        0xEB80,	// K 1110 1011 1
        0xBA80,	// L 1011 1010 1
        0xEE00,	// M 1110 111
        0xE800,	// N 1110 1
        0xEEE0,	// O 1110 1110 111
        0xBBA0,	// P 1011 1011 101
        0xEEB8,	// Q 1110 1110 1011 1
        0xBA00,	// R 1011 101
        0xA800,	// S 1010 1
        0xE000,	// T 111
        0xAE00,	// U 1010 111
        0xAB80,	// V 1010 1011 1
        0xBB80,	// W 1011 1011 1
        0xEAE0,	// X 1110 1010 111
        0xEBB8,	// Y 1110 1011 1011 1
        0xEEA0	// Z 1110 1110 101
};
static char letter_toupper(char letter){
    // convert lowercase to uppercase
    if(letter > 'a' && letter < 'z'){
        return (char)(letter - 32);
    }
    return letter;
}
static void string_to_morse(char letter){
    // string to morse + led response
    unsigned short morse_code;
    int dot_counter;
    char uppercase = letter;

    //not a letter
    if(uppercase < 'A' || uppercase > 'Z') return;

    // create a binary for morse-code from morse-code table
    morse_code = morsecode_codes[uppercase - 65];

    // counter to determine it is a dot or dash
    dot_counter = 0;
    while(morse_code || dot_counter > 0){
        //morse_code and 1000 0000 0000 0000

        // 1
        if(morse_code & 0x8000){
            my_led_on();
            dot_counter+=1;
//            printk(KERN_INFO "sleep 1  (step 1)\n");
            msleep(DOT_TIME);
        }
        // 0
        else if(dot_counter > 0){
            my_led_off();
            //dot
            if(dot_counter == 1){
                if (!kfifo_put(&echo_fifo, '.')) return;
            }
            //dash
            else if(dot_counter==3) {
                if (!kfifo_put(&echo_fifo, '-')) return;
            }
            //reset counter
            dot_counter=0;
            if(morse_code){
//                printk(KERN_INFO "sleep 1  (step 2)\n");
                msleep(DOT_TIME);
            }
        }
        //shift one to left
        morse_code <<=1;
    }
}

/******************************************************
 * Callbacks
 ******************************************************/
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/ctype.h>

static ssize_t my_read(struct file *file,
                         char *buf, size_t count, loff_t *ppos)
{
    // Pull all available data from fifo into user buffer
    int num_bytes_read = 0;
    printk(KERN_INFO "morse::my_read(), buff size %d, f_pos %d\n",
            (int) count, (int) *ppos);

    if (kfifo_to_user(&echo_fifo, buf, count, &num_bytes_read)) {
        printk(KERN_ERR "Unable to read from buffer.");
        return -EFAULT;
    }

    return num_bytes_read;  // # bytes actually read.
}
static ssize_t my_write(struct file *file,
                          const char *buff, size_t count, loff_t *ppos)
{
    int start;
    int end;
    int i;
    char letter;
    char previous_letter = ' ';

    char *morse_string;

    printk(KERN_INFO "morse: Start encode morse code %d\n", count);
    // Push data into fifo, one byte at a time (with delays)

    start = 0;
    end = (int)count - 1;

    //build string
    morse_string = kmalloc((int)count, GFP_KERNEL);
    for (i = 0; i < count; i++) {
        // input
        if (copy_from_user(&letter, &buff[i], sizeof(letter))) {
            printk(KERN_ERR "morse: Unable to write to buffer.");
            return -EFAULT;
        }
        morse_string[i] = letter_toupper(letter);
    }

    // Trim whitespace from the front and end of the input
    while(morse_string[start] == ' '){
        printk(KERN_INFO "morse: string start%c\n", morse_string[start]);
        start++;
    }
    while (end >= start && (morse_string[end] == ' ' || morse_string[end] == '\n')){
        printk(KERN_INFO "morse: string end %c\n", morse_string[end]);
        end--;
    }
    //string to morse code
    for(i = start; i < end+1; i++){
        //if it is a space and

        printk(KERN_INFO "morse: current letter: %c\n", morse_string[i]);
        if(morse_string[i] == ' ' && (previous_letter >= 'A' && previous_letter <= 'Z')){
            my_led_off();
            if (!kfifo_put(&echo_fifo, ' ')) return -EFAULT;
            printk(KERN_INFO "sleep 4  (step 3 space)\n");
            msleep(DOT_TIME * 4);
            previous_letter = morse_string[i];
            continue;
        }
        else if(morse_string[i] == ' ' && previous_letter == ' '){
            previous_letter = morse_string[i];
            continue;
        }

        //encode morse
        string_to_morse(morse_string[i]);

        //next letter and ignore the last delay and space
        if(i != end){
            if (!kfifo_put(&echo_fifo, ' ')) return -EFAULT;
            //previous is a letter
            printk(KERN_INFO "sleep 3  (step 4 next letter)\n");
            msleep(DOT_TIME * 3);
        }

        previous_letter = morse_string[i];
    }
    // insert break line
    if (!kfifo_put(&echo_fifo, '\\')) return -EFAULT;
    if (!kfifo_put(&echo_fifo, 'n')) return -EFAULT;
    if (!kfifo_put(&echo_fifo, '\n')) return -EFAULT;
    my_led_off();
    printk(KERN_INFO "morse: encode finished %d\n", count);
    // Return # bytes actually written.
    return count;
}


/******************************************************
 * Misc support
 ******************************************************/
// Callbacks:  (structure defined in <kernel>/include/linux/fs.h)
struct file_operations my_fops = {
	.owner    =  THIS_MODULE,
    .read     =  my_read,
	.write    =  my_write,
};

// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,             // /dev/.... file.
		.fops     = &my_fops                    // Callback functions.
};


/******************************************************
 * Driver initialization and exit:
 ******************************************************/
static int __init my_init(void)
{
	int ret;
	printk(KERN_INFO "----> morse driver init(): file /dev/%s.\n", MY_DEVICE_FILE);

	// Register as a misc driver:
	ret = misc_register(&my_miscdevice);

	// LED:
	led_register();

    // Initialize the FIFO.
    INIT_KFIFO(echo_fifo);

	return ret;
}

static void __exit my_exit(void)
{
	printk(KERN_INFO "<---- morse driver exit().\n");

	// Unregister misc driver
	misc_deregister(&my_miscdevice);

	// LED:
	led_unregister();
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Andrew Liang");
MODULE_DESCRIPTION("morse LED driver");
MODULE_LICENSE("GPL");
