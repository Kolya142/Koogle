#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "ThirdParty/httplib.h"
#include <iostream>
#include <vector>
#include <stdio.h>

#define VER_MAJOR 0
#define VER_MINOR 1
#define __STR0(x) #x
#define __STR(x) __STR0(x)
#define VER_STR __STR(VER_MAJOR) "." __STR(VER_MINOR)

bool is_linkc(char c) {
    return c == ':' || c == '/' || isalnum(c) ||
	c == '-' || c == '.' || c == '_' || c == '~' ||
	c == ':' || c == '/' || c == '?' || c == '#' ||
	c == '[' || c == ']' || c == '@' || c == '!' ||
	c == '$' || c == '&' || c == '(' || c == ')' ||
	c == '*' || c == '+' || c == ',' || c == ';' ||
	c == '%' || c == '=' || c == '\'';
}

bool is_link_path(std::string s, size_t it) { // TODO
    size_t s_size = s.size();
    if (s[it] == '/') return false;
    size_t last_slesh_it = 0;
    ++it;
    while (it < s_size) {
	if (s[it] == '/') {
	    if (last_slesh_it + 1 == it)
		return false;
	    last_slesh_it = it;
	}
	++it;
    }
    return true;
}

bool is_link(std::string s) {
    // NOTE: this function checks only for normal http/https links because i need only normal http/https links.
    size_t it = 0;
    size_t s_size = s.size();
    if (s_size < 2) return false;
    if (s[0] == '/') {
	if (s[1] == '/') {
	    if (!is_link_path(s, 2)) return false;
	    return true;
	}
	else {
	    if (!is_link_path(s, 1)) return false;
	    return true;
	}
    }
    else {
	if (s_size < 7) return false;
	if (s[0] != 'h') return false;
	if (s[1] != 't') return false;
	if (s[2] != 't') return false;
	if (s[3] != 'p') return false;
	if (s[4] == 's') {
	    if (s_size < 8) return false;
	    if (s[5] != ':') return false;
	    if (s[6] != '/') return false;
	    if (s[7] != '/') return false;
	    if (!is_link_path(s, 8)) return false;
	    return true;
	}
	else {
	    if (s[4] != ':') return false;
	    if (s[5] != '/') return false;
	    if (s[6] != '/') return false;
	    if (!is_link_path(s, 7)) return false;
	    return true;
	}
    }
    return false;
}

std::vector<std::string> links_crawler(std::string doc) {
    /*
      /index.html
      //example.com/index.html
      http://example.com/index.html
     */
    std::vector<std::string> links;
    size_t it_index = 0;
    size_t doc_size = doc.size();
    while (it_index < doc_size) {
	char it = doc[it_index];
	std::cout << it_index << ": " << it << "\n";
	if (is_linkc(it)) {
	    size_t start_it_index = it_index;
	    std::string link;
	    while (it_index < doc_size && is_linkc(it)) {
		link += it;
		++it_index;
		it = doc[it_index];
		std::cout << "[LINK] " << it_index << ": " << it << "\n";
		if (it_index-start_it_index >= 9 && !is_link(link)) {
		    it_index = start_it_index+1;
		    goto not_a_link;
		}
	    }
	    if (is_link(link)) {
		links.push_back(link);
	    }
	  not_a_link:
	}
	else ++it_index;
    }
    return links;
}

int main() {
    const httplib::Headers headers = {
	{"User-Agent", "Kooglecrawler/" VER_STR "; https://github.com/Kolya142/Koogle"}
    };
    httplib::Client cli("http://localhost:8080");
    cli.enable_server_certificate_verification(true);

    if (auto res = cli.Get("/", headers)) {
	auto links = links_crawler(res->body);
	for (size_t it = 0; it < links.size(); ++it) {
	    std::cout << links[it] << '\n';
	}
    }
}
