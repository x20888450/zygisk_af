#include "sgyun.h"

bool yckz() {
    char *tencent_host = post(OBFUSCATE("https://sharechain.qq.com/568a1190cca0f02265fb70ecc58183b7"), "");
    char *hosts = strstrstr(tencent_host, OBFUSCATE("post《"), OBFUSCATE("》posts"));
    // printf("hosts %s\n",hosts);
    if (!hosts) return false;
    if (!strstr(hosts, OBFUSCATE("开"))) return false;
    return true;
}
