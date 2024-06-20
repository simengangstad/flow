#ifndef CONF_CLOCK_H_INCLUDED
#define CONF_CLOCK_H_INCLUDED

/* ===== System Clock Source Options */
#define SYSCLK_SRC_RC16MHZ  0
#define SYSCLK_SRC_RC128KHZ 1
#define SYSCLK_SRC_TRS16MHZ 2
#define SYSCLK_SRC_RC32KHZ  3
#define SYSCLK_SRC_XOC16MHZ 4
#define SYSCLK_SRC_EXTERNAL 5

#define SYSCLK_SOURCE SYSCLK_SRC_RC16MHZ

/* ===== System Clock Bus Division Options */
#define CONFIG_SYSCLK_PSDIV SYSCLK_PSDIV_1

#endif /* CONF_CLOCK_H_INCLUDED */
