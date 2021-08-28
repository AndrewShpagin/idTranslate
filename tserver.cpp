#include "tserver.h"

int main()
{
	///  First, create the translation server, you need to do it once.
	translate::tServer tr("public/edittext.html", 5678);
	/// Setup the translation callback. Pay attention, it may be called in the separate thread, 
	/// so be careful with the multithreading issues.
	tr.onTranslate([](const json::JSON& res) {
		std::cout << "Got translation:\n" << res.dump();
	});
	
	/// The next commands, addText ...show may be repeated many times asyncronously.
	/// Add texts to the translation queue
	tr.addText("HELLO", "Hello, world!", "Hello, world!");
	tr.addText("HOWAREYOU", "How are you?", "How are you?");
	/// Show the translation dialog
	tr.show("Andrew", "andrewshpagin@gmail.com", "Japanese", "ja");
	///Just wait.
	std::system("pause");
	return 0;
}
