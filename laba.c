#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/memblock.h>
#include <asm/thread_info.h>
#include <asm/processor.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>

#define MAX_DEBUGFS_SIZE 100
#define DEBUGFS_INTERFACE_DIR "abc"
#define DEBUGFS_CPU_TIMER_INTERFACE_FILE "ct"
#define DEBUGFS_SIGNAL_STRUCT_INTERFACE_FILE "sig"


static char ts_buffer[MAX_DEBUGFS_SIZE];
static char vma_buffer[MAX_DEBUGFS_SIZE];
static unsigned long ct_buffer_size = 0;
static unsigned long sig_buffer_size = 0;

ssize_t ct_read_interface(struct file * file, char __user * buff, size_t count, loff_t * offset) {
	
	static int read_status = 0;
	if(read_status != 0) {
		read_status = 0;
		return 0;
	}

	rcu_read_lock();
	long seeked_pid = 1;
	kstrtol(ts_buffer, 10, &seeked_pid);
	struct task_struct *task;
	int found_flag = 0;
	for_each_process(task) {
		if(task->pid == seeked_pid) {
			found_flag = 1;
			break;
		}
	}
	if(found_flag == 1) {
		struct timerqueue_node tqn = task->signal->real_timer.node;
		struct timerqueue_head tqh = task->posix_cputimers.bases[0].tqhead;
		const char fields_values_str[MAX_DEBUGFS_SIZE];
		const char format_answer[] = "Timerqueue node expires: %u\nTimer PID: %u\nTimerqueue head pointer: %x\n";
		size_t string_size = snprintf(NULL, 0, format_answer, tqn.expires, seeked_pid, &tqh) + 1;
		snprintf(fields_values_str, string_size, format_answer, tqn.expires, seeked_pid, &tqh);
		copy_to_user(buff, fields_values_str, string_size);
		read_status = string_size;
	}
	else {
		copy_to_user(buff, "Nothing found\n", 15);
		read_status = 15;
	}
	rcu_read_unlock();

	return read_status;
}


ssize_t ct_write_interface(struct file * file, const char __user * buff, size_t count, loff_t * offset) {
	if(count <= MAX_DEBUGFS_SIZE) ct_buffer_size = count;
	else ct_buffer_size = MAX_DEBUGFS_SIZE;
	copy_from_user(ts_buffer, buff, ct_buffer_size);
	return ct_buffer_size;
}


ssize_t sig_read_interface(struct file * file, char __user * buff, size_t count, loff_t * offset) {
	static int read_status = 0;
	if(read_status != 0) {
		read_status = 0;
		return 0;
	}

	rcu_read_lock();
	long seeked_pid = 1;
	kstrtol(ts_buffer, 10, &seeked_pid);
	struct task_struct *task;
	int found_flag = 0;
	for_each_process(task) {
		if(task->pid == seeked_pid) {
			found_flag = 1;
			break;
		}
	}
	if(found_flag == 1) {
		struct signal_struct * sig = task->signal;
		const char fields_values_str[MAX_DEBUGFS_SIZE];
		const char format_answer[] = "Nr threads: %u\nNotify count: %u\nGroup stop count: %u\nThread head: %x\nLive: %u\n";
		size_t string_size = snprintf(NULL, 0, format_answer, sig->nr_threads, sig->notify_count, sig->group_stop_count, &(sig->thread_head), sig->live.counter) + 1;
		snprintf(fields_values_str, string_size, format_answer, sig->nr_threads, sig->notify_count, sig->group_stop_count, &(sig->thread_head), sig->live.counter);
		copy_to_user(buff, fields_values_str, string_size);
		read_status = string_size;
	}
	else {
		copy_to_user(buff, "Nothing found\n", 15);
		read_status = 15;
	}
	rcu_read_unlock();

	return read_status;
}


ssize_t sig_write_interface(struct file * file, const char __user * buff, size_t count, loff_t * offset) {
	if(count <= MAX_DEBUGFS_SIZE) sig_buffer_size = count;
	else sig_buffer_size = MAX_DEBUGFS_SIZE;
	copy_from_user(vma_buffer, buff, sig_buffer_size);
	return sig_buffer_size;
}

static const struct file_operations ts_file_interface = {
	.owner=THIS_MODULE,
	.read=ct_read_interface,
	.write=ct_write_interface
};

static const struct file_operations vma_file_interface = {
	.owner=THIS_MODULE,
	.read=sig_read_interface,
	.write=sig_write_interface
};

static int __init initer(void) {
	struct dentry* interface_dir = debugfs_create_dir(DEBUGFS_INTERFACE_DIR, NULL);
	debugfs_create_file(DEBUGFS_CPU_TIMER_INTERFACE_FILE, 0777, interface_dir, NULL, &ts_file_interface);
	debugfs_create_file(DEBUGFS_SIGNAL_STRUCT_INTERFACE_FILE, 0777, interface_dir, NULL, &vma_file_interface);
	return 0;
}

static void __exit goodbye(void) {
}

module_init(initer);
module_exit(goodbye);

MODULE_LICENSE("GPL");
