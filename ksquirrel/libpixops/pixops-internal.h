#ifdef USE_MMX

unsigned char *pixops_scale_line_22_33_mmx(unsigned int weights[16][8], 
					    unsigned char *p,
					    unsigned char *q1,
					    unsigned char *q2,
					    int x_step,
					    unsigned char *p_stop,
					    int x_init);

unsigned char *pixops_composite_line_22_4a4_mmx(unsigned int weights[16][8],
						    unsigned char *p,
						    unsigned char *q1,
						    unsigned char *q2,
						    int x_step,
						    unsigned char *p_stop,
						    int x_init);

unsigned char *pixops_composite_line_color_22_4a4_mmx(unsigned int weights[16][8],
							unsigned char *p,
							unsigned char *q1,
							unsigned char *q2,
							int x_step,
							unsigned char *p_stop,
							int x_init,
							int dest_x,
							int check_shift,
							int *colors);

int pixops_have_mmx (void);

#endif

