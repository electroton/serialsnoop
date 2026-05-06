#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdint.h>

#define CONFIG_DEVICE_LEN   64
#define CONFIG_LOGFILE_LEN  256
#define CONFIG_PROTOCOL_LEN 32

/* Supported parity modes */
typedef enum {
    PARITY_NONE = 0,
    PARITY_ODD,
    PARITY_EVEN
} parity_mode_t;

/* Main configuration structure */
typedef struct {
    char        device[CONFIG_DEVICE_LEN];       /* Serial device path */
    char        logfile[CONFIG_LOGFILE_LEN];     /* Output log file path */
    char        protocol[CONFIG_PROTOCOL_LEN];   /* Protocol filter name */
    uint32_t    baud_rate;                       /* Baud rate (e.g. 115200) */
    uint8_t     data_bits;                       /* Data bits: 5,6,7,8 */
    uint8_t     stop_bits;                       /* Stop bits: 1 or 2 */
    parity_mode_t parity;                        /* Parity mode */
    int         verbose;                         /* Verbose output flag */
    int         hex_dump;                        /* Enable hex dump output */
    int         timestamps;                      /* Prepend timestamps */
    uint32_t    filter_mask;                     /* Packet type filter mask */
} config_t;

/**
 * Initialize config with safe defaults.
 * Default: 115200 8N1, no log file, no filters.
 */
void config_init(config_t *cfg);

/**
 * Parse a key=value config file into cfg.
 * Lines starting with '#' are treated as comments.
 * Returns 0 on success, -1 if file cannot be opened.
 */
int config_parse_file(config_t *cfg, const char *path);

/**
 * Validate that cfg contains a usable configuration.
 * Returns 0 if valid, -1 with reason printed to stderr.
 */
int config_validate(const config_t *cfg);

/**
 * Print current configuration to stdout (for --show-config).
 */
void config_dump(const config_t *cfg);

#endif /* CONFIG_PARSER_H */
