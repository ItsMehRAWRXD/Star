#include <stdio.h>

void track_event(const char* event, const char* user) {
    printf("TRACK: event=%s user=%s\n", event, user);
}

int main(void) {
    track_event("app-event", "user-d4e87ad2");
    return 0;
}