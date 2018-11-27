#ifndef _HTTP_HANDLER_H_
#define _HTTP_HANDLER_H_

void http_handler_loop();
bool http_start_download(char * url);
bool http_handle_get_stream(FixedLengthAccumulator& dst);

#endif
