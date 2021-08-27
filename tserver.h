#pragma once
#include <functional>
#include "httplib.h"
#include "json.h"
#include "jcc.h"
namespace translate {
	class tServer {
		jcc::LocalServer* ls;
		std::thread* thread;
		std::mutex m;
		std::string html_path;
		std::string tranlationServer;
		json::JSON toTranslate;
		int tidx;
		std::vector<std::string> buttons;
	public:
		tServer(const char* path_to_edittext_html, int preferred_port = -1);
		~tServer();
		/// Add the text into the translation queue, id it the text identifier, EngText is a current English text equivalent for the id, Translation is the current translation to the corresponding language	
		void addText(const char* id, const char* EngText, const char* Translation);
		/// add the button to the translation dialog, the name of this button (if pressed) will appear in the resulting JSON, see onTranslate.
		/// You need to add all required buttons before the show(), after show() the buttons list cleared.
		void addButton(const char* button);
		/// Show the translation dialog
		void show(const char* author, const char* author_email, const char* LanguageFullName, const char* LanguageShortName);
		///When the user ended the translation and pressed submit, the result will be passed to this function. Use std::cout << result.dump() to understand the output and access the result.
		void onTranslate(std::function<void(const json::JSON& result)>);
		///The optional functionality to provide on-the-fly translation by google/DeepL/Bing. It is not mandatory. Use it only if you need really advanced level of usage of this module.
		///The idea: Deploy the translation service (for example, using Heroku) that will translate texts on-the-fly.
		void setTranslationServer(const char* translationServerURL);
	};


	inline tServer::tServer(const char* path_to_edittext_html, int preferred_port) {
		html_path = path_to_edittext_html;
		ls = new jcc::LocalServer(preferred_port);
		tidx = 0;
		thread = new std::thread([=] {
			ls->listen();
			});
		thread->detach();
	}

	inline tServer::~tServer() {
		delete(thread);
		delete(ls);
	}

	inline void tServer::addText(const char* id, const char* EngText, const char* Translation) {
		std::string s = "TextItem" + std::to_string(tidx++);
		json::JSON& txt = toTranslate[s] = json::Object();
		txt["ID"] = id;
		txt["Text"] = Translation;
		txt["English"] = EngText;
	}

	inline void tServer::addButton(const char* button) {
		buttons.push_back(button);
	}

	inline void tServer::show(const char* author, const char* author_email, const char* LanguageFullName, const char* LanguageShortName) {
		jcc::Html h(html_path.c_str());
		for (int i = 0; i < toTranslate.size(); i++) {
			std::string s = "TextItem" + std::to_string(i);
			toTranslate[s]["tl"] = LanguageShortName;
		}
		json::JSON rs;
		rs = toTranslate;
		rs["Language"] = LanguageFullName;
		rs["author"] = author;
		rs["mail"] = author_email;
		if (buttons.size()) {
			for (int i = 0; i < buttons.size(); i++) {
				std::string s = "Button" + std::to_string(i);
				rs[s] = buttons[i];
			}
		}
		else {
			rs["Button"] = "Submit";
		}
		h.Replace("{JSONTEXT}", rs.dump().c_str());
		toTranslate = json::Object();
		tidx = 0;
		ls->open(h);
	}

	inline void tServer::onTranslate(std::function<void(const json::JSON& result)> fn) {
		ls->get([=](const jcc::Request& req, jcc::Response& res) {
			res = "<html><body><div>Text accepted! Please close the page if it is not closed automatically.</div><script>window.close();</script></body></html>";
			fn(req.paramsToJson());
			}, "/submit");
	}

	inline void tServer::setTranslationServer(const char* translationServerURL) {
		tranlationServer = translationServerURL;
	}
}