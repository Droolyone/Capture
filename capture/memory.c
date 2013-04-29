/*
 * Check memory range for valid RAM. A simple memory test determines
 * the actually available RAM size between addresses `base' and
 * `base + maxsize'.
 * Copied and pasted from barebox common/memsize.c file
 */
long get_ram_size(long *base, long maxsize)
{
	volatile long *addr;
	long           cnt;
	long           val;
	long           size;
	int            i = 0;

	for (cnt = (maxsize / sizeof (long)) >> 1; cnt > 0; cnt >>= 1) {
		addr = base + cnt;	/* pointer arith! */
		*addr = ~cnt;
	}

	addr = base;
	*addr = 0;

	for (cnt = 1; cnt < maxsize / sizeof (long); cnt <<= 1) {
		addr = base + cnt;	/* pointer arith! */
		val = *addr;
		if (val != ~cnt) {
			size = cnt * sizeof (long);
			return (size);
		}
	}

	return (maxsize);
}
