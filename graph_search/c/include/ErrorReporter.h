#pragma once

void ErrorReporter_Report(int, char*);
int ErrorReporter_LastErrorCode();
char* ErrorReporter_LastErrorMessage();
void ErrorReporter_Destory();
