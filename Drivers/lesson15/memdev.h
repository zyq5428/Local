#define MEM_MAGIC 'm'
#define MEM_RESTART _IO(MEM_MAGIC,0)
#define MEM_SET _IOW(MEM_MAGIC,1,int)

