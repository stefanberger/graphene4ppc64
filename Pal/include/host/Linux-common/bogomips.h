#ifndef BOGOMIPS_H
#define BOGOMIPS_H

double get_bogomips_from_cpuinfo_buf(const char* buf, size_t size);
double sanitize_bogomips_value(double);

char* get_cpu_from_cpuinfo_buf(const char* buf);

#endif // BOGOMIPS_H

