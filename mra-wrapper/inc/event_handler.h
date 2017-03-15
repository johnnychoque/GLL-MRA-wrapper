// Mode:C++; c-basic-indent:4 -*- //

#ifndef _event_handler_h
#define _event_handler_h


class EventHandler {
 public:
    virtual void onEvent(struct eventMessage *message) = 0;
    virtual ~EventHandler(){}
};


#endif // _event_handler_h
