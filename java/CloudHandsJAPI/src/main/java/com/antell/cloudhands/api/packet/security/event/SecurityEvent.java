package com.antell.cloudhands.api.packet.security.event;

import com.antell.cloudhands.api.source.SourceEntry;

public abstract class SecurityEvent {

    private long dateStart;
    private long dateEnd;

    private long eventID;
    private String eventType;
    private String eventEngine;
    private SourceEntry sourceEntry;

    public SecurityEvent(SourceEntry sourceEntry,long dateStart,long dateEnd,long eventID,String eventType,String eventEngine){

        this.sourceEntry = sourceEntry;
        this.dateStart = dateStart;
        this.dateEnd = dateEnd;
        this.eventID = eventID;
        this.eventType = eventType;
        this.eventEngine = eventEngine;
    }

    public long getDateStart() {
        return dateStart;
    }

    public void setDateStart(long dateStart) {
        this.dateStart = dateStart;
    }

    public long getDateEnd() {
        return dateEnd;
    }

    public void setDateEnd(long dateEnd) {
        this.dateEnd = dateEnd;
    }

    public long getEventID() {
        return eventID;
    }

    public void setEventID(long eventID) {
        this.eventID = eventID;
    }

    public String getEventType() {
        return eventType;
    }

    public void setEventType(String eventType) {
        this.eventType = eventType;
    }

    public String getEventEngine() {
        return eventEngine;
    }

    public void setEventEngine(String eventEngine) {
        this.eventEngine = eventEngine;
    }
}
