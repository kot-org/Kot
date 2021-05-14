#pragma once
typedef struct {
	void* PowerOff;
	void* Restart;
} PowerInfo;

class Power {
public:
    void InitPower(void* pdwn,void* rest);
    void Shutdown();
    void Restart();
};