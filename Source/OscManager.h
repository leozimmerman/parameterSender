
#pragma once

#define DEFAULT_OSC_HOST "127.0.0.1"
#define DEFAULT_OSC_PORT 9001

class OscManager {
public:
    
    OscManager() {
        _oscHost = DEFAULT_OSC_HOST;
        _oscPort = DEFAULT_OSC_PORT;
        _mainID = "parameterSender";
    }
    
    void setMaindId(juce::String mainId) {
        _mainID = mainId;
    }
    
    void setOscPort(int port) {
        _oscPort = port;
        connect();
    }
    
    void setOscHost(juce::String hostAdress) {
        _oscHost = hostAdress;
        connect();
    }
    
    void connect() {
        connect(_oscHost, _oscPort);
    }
    
    void connect(const juce::String& targetHostName, int targetPortNumber) {
        oscSender.disconnect();
        if (! oscSender.connect (targetHostName, targetPortNumber)) {
            juce::Logger::outputDebugString(&"Error: could not connect to UDP port:" [ targetPortNumber]);
        }
    }
    
    void sendValue(float value, juce::String name) {
        juce::String root = "/" + _mainID;
        juce::String address = root + "/" + name;
        juce::OSCAddressPattern addressPattern = juce::OSCAddressPattern(address);
        oscSender.send(addressPattern, value);
    }
    
private:
    juce::OSCSender oscSender;
    juce::String _oscHost;
    juce::String _mainID;
    int _oscPort;
};
