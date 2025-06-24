#include <stdarg.h>
#include <string.h>
#include <orbis/libkernel.h>

#define TEX_ICON_SYSTEM "/user/data/icon0.png"

// For formatted strings
static void Notify(const char* IconUri, const char* FMT, ...)
{
    OrbisNotificationRequest Buffer = {};
    va_list args;
    va_start(args, FMT);
    const int len = vsprintf(Buffer.message, FMT, args);
    const int len2 = len - 1;
    if (len > 0 && Buffer.message[len2] == '\n')
    {
        Buffer.message[len2] = 0;
    }
    va_end(args);
    printf("Notify message:\n%s\n", Buffer.message);
    Buffer.type = NotificationRequest;
    Buffer.unk3 = 0;
    Buffer.useIconImageUri = 1;
    Buffer.targetId = -1;
    if (IconUri && *IconUri)
    {
        memcpy(Buffer.iconUri, IconUri, sizeof(Buffer.iconUri));
    }
    else
    {
        memcpy(Buffer.iconUri, TEX_ICON_SYSTEM, sizeof(TEX_ICON_SYSTEM));
    }
    sceKernelSendNotificationRequest(0, &Buffer, sizeof(Buffer), 0);
}

#define printf_notification3(icon_uri, ...)    {                                                    \
  SceNotificationRequest noti_buffer;                                                                \
  noti_buffer.type = 0;                                                                                \
  noti_buffer.unk3 = 0;                                                                                \
  noti_buffer.use_icon_image_uri = 1;                                                                  \
  noti_buffer.target_id = -1;                                                                          \
  snprintf_s(noti_buffer.uri, sizeof(noti_buffer.uri), icon_uri);                                      \
  snprintf_s(noti_buffer.message, sizeof(noti_buffer.message), ##__VA_ARGS__);                         \
  printf_debug("[NOTIFICATION]: %s\n", noti_buffer.message);                                            \
  sceKernelSendNotificationRequest(0, (SceNotificationRequest *)&noti_buffer, sizeof(noti_buffer), 0); \
}