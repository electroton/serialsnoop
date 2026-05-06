#include "config_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define DEFAULT_BAUDRATE      115200
#define DEFAULT_DATA_BITS     8
#define DEFAULT_STOP_BITS     1
#define DEFAULT_PARITY        'N'
#define DEFAULT_RING_BUF_SIZE 4096

void config_init(config_t *cfg)
{
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
    cfg->baudrate         = DEFAULT_BAUDRATE;
    cfg->data_bits        = DEFAULT_DATA_BITS;
    cfg->stop_bits        = DEFAULT_STOP_BITS;
    cfg->parity           = DEFAULT_PARITY;
    cfg->hex_dump_enabled = 1;
    cfg->timestamps_enabled = 1;
    cfg->stats_enabled    = 0;
    cfg->filter_enabled   = 0;
    cfg->ring_buffer_size = DEFAULT_RING_BUF_SIZE;
    strncpy(cfg->protocol, "raw", sizeof(cfg->protocol) - 1);
}

static void trim(char *s)
{
    char *end;
    while (isspace((unsigned char)*s)) memmove(s, s + 1, strlen(s));
    end = s + strlen(s) - 1;
    while (end >= s && isspace((unsigned char)*end)) *end-- = '\0';
}

int config_parse_line(config_t *cfg, const char *key, const char *value)
{
    if (!cfg || !key || !value) return -1;

    if      (strcmp(key, "device")      == 0) { strncpy(cfg->device,   value, CONFIG_MAX_DEVICE_LEN  - 1); }
    else if (strcmp(key, "baudrate")    == 0) { cfg->baudrate         = (uint32_t)atol(value); }
    else if (strcmp(key, "data_bits")   == 0) { cfg->data_bits        = (uint8_t)atoi(value); }
    else if (strcmp(key, "stop_bits")   == 0) { cfg->stop_bits        = (uint8_t)atoi(value); }
    else if (strcmp(key, "parity")      == 0) { cfg->parity           = toupper((unsigned char)value[0]); }
    else if (strcmp(key, "hex_dump")    == 0) { cfg->hex_dump_enabled = atoi(value); }
    else if (strcmp(key, "timestamps")  == 0) { cfg->timestamps_enabled = atoi(value); }
    else if (strcmp(key, "stats")       == 0) { cfg->stats_enabled    = atoi(value); }
    else if (strcmp(key, "logfile")     == 0) { strncpy(cfg->logfile,  value, CONFIG_MAX_LOGFILE_LEN - 1); }
    else if (strcmp(key, "protocol")    == 0) { strncpy(cfg->protocol, value, CONFIG_MAX_PROTOCOL_LEN - 1); }
    else if (strcmp(key, "filter")      == 0) { cfg->filter_enabled   = atoi(value); }
    else if (strcmp(key, "ring_buffer_size") == 0) { cfg->ring_buffer_size = (uint32_t)atol(value); }
    else return -1;

    return 0;
}

int config_parse_file(config_t *cfg, const char *path)
{
    FILE *fp;
    char  line[512];
    int   lineno = 0;

    if (!cfg || !path) return -1;
    fp = fopen(path, "r");
    if (!fp) { fprintf(stderr, "config: cannot open '%s': %s\n", path, strerror(errno)); return -1; }

    while (fgets(line, sizeof(line), fp)) {
        char *eq, *key, *value;
        lineno++;
        trim(line);
        if (line[0] == '\0' || line[0] == '#' || line[0] == '[') continue;
        eq = strchr(line, '=');
        if (!eq) { fprintf(stderr, "config:%d: malformed line\n", lineno); continue; }
        *eq   = '\0';
        key   = line;
        value = eq + 1;
        trim(key);
        trim(value);
        if (config_parse_line(cfg, key, value) != 0)
            fprintf(stderr, "config:%d: unknown key '%s'\n", lineno, key);
    }
    fclose(fp);
    return 0;
}

int config_validate(const config_t *cfg)
{
    if (!cfg) return -1;
    if (cfg->device[0] == '\0') { fprintf(stderr, "config: 'device' is required\n"); return -1; }
    if (cfg->baudrate == 0)      { fprintf(stderr, "config: invalid baudrate\n");      return -1; }
    if (cfg->data_bits < 5 || cfg->data_bits > 8) { fprintf(stderr, "config: data_bits must be 5-8\n"); return -1; }
    if (cfg->parity != 'N' && cfg->parity != 'E' && cfg->parity != 'O') { fprintf(stderr, "config: parity must be N/E/O\n"); return -1; }
    if (cfg->ring_buffer_size == 0) { fprintf(stderr, "config: ring_buffer_size must be > 0\n"); return -1; }
    return 0;
}

void config_dump(const config_t *cfg)
{
    if (!cfg) return;
    printf("[serialsnoop config]\n");
    printf("  device           = %s\n",  cfg->device);
    printf("  baudrate         = %u\n",  cfg->baudrate);
    printf("  data_bits        = %u\n",  cfg->data_bits);
    printf("  stop_bits        = %u\n",  cfg->stop_bits);
    printf("  parity           = %c\n",  cfg->parity);
    printf("  protocol         = %s\n",  cfg->protocol);
    printf("  hex_dump         = %d\n",  cfg->hex_dump_enabled);
    printf("  timestamps       = %d\n",  cfg->timestamps_enabled);
    printf("  stats            = %d\n",  cfg->stats_enabled);
    printf("  filter           = %d\n",  cfg->filter_enabled);
    printf("  ring_buffer_size = %u\n",  cfg->ring_buffer_size);
    if (cfg->logfile[0]) printf("  logfile          = %s\n", cfg->logfile);
}
