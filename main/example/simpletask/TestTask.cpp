
#include "TestTask.h"

namespace example::simpletask
{

	void launch() {

		printf("Inicio TTN!\n");

		MyTask* pMyTask1 = new MyTask("uno", 1000);
		pMyTask1->setStackSize(20000);
		pMyTask1->start();

		MyTask* pMyTask2 = new MyTask("dossssssss", 2000);
		pMyTask2->setStackSize(20000);
		pMyTask2->start();

		printf("Final de la tarea principal!");

	}

}