#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdint.h>

#define CONFIG_MAX_DEVICE_LEN  64
#define CONFIG_MAX_LOGFILE_LEN 256
#define CONFIG_MAX_PROTOCOL_LEN 32

typedef struct {
    char     device[CONFIG_MAX_DEVICE_LEN];
    uint32_t baudrate;
    uint8_t  data_bits;
    uint8_t  stop_bits;
    char     parity;           /* 'N', 'E', 'O' */
    int      hex_dump_enabled;
    int      timestamps_enabled;
    int      stats_enabled;
    char     logfile[CONFIG_MAX_LOGFILE_LEN];
    char     protocol[CONFIG_MAX_PROTOCOL_LEN];
    int      filter_enabled;
    uint32_t ring_buffer_size;
} config_t;

/**
 * Initialize config with sensible defaults.
 */
void config_init(config_t *cfg);

/**
 * Parse an INI-style config file.
 * Returns 0 on success, -1 on error.
 */
int config_parse_file(config_t *cfg, const char *path);

/**
 * Parse a single key=value line into the config.
 * Returns 0 if recognized, -1 if unknown key.
 */
int config_parse_line(config_t *cfg, const char *key, const char *value);

/**
 * Validate the config for required fields and sane values.
 * Returns 0 if valid, -1 with an error written to stderr.
 */
int config_validate(const config_t *cfg);

/**
 * Print the current config to stdout (for --dump-config).
 */
void config_dump(const config_t *cfg);

#endif /* CONFIG_PARSER_H */
