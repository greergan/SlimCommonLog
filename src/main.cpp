#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <string_view>
#include <slim/common/log.h>
namespace slim::common::log {
	std::mutex cerr_mutex;
	std::mutex cout_mutex;
	std::unordered_map<std::string_view, std::string_view> color_codes {
		{"black",   "\033[0;30m"},
		{"blue",    "\033[0;34m"},
		{"cyan",    "\033[0;36m"},
		{"green",   "\033[0;32m"},
		{"magenta", "\033[0;35m"},
		{"red",     "\033[0;31m"},
		{"white",   "\033[0;37m"},
		{"yellow",  "\033[0;33m"}
	};
	std::unordered_map<std::string_view, std::string_view> colors {
		{"END",     "\033[0m"},
		{"DEBUG",   color_codes["yellow"]},
		{"ERROR",   color_codes["red"]},
		{"FILE",    color_codes["magenta"]},
		{"FUNCTION",color_codes["green"]},
		{"INFO",    color_codes["white"]},
		{"LINE",    color_codes["magenta"]},
		{"TEXT",    color_codes["white"]},
		{"TRACE",   color_codes["cyan"]}
	};
	std::function<bool(std::string_view, std::string_view, std::string_view, std::string_view)> can_log =
	[]([[maybe_unused]] std::string_view consumer,
        [[maybe_unused]] std::string_view log_level,
        [[maybe_unused]] std::string_view file,
        [[maybe_unused]] std::string_view function) {
		return true;
    };
}
void slim::common::log::set_can_log(std::function<bool(
		std::string_view consumer, std::string_view log_level, std::string_view file, std::string_view function)> function) {
	can_log = function;
}
void slim::common::log::debug(Message message) {
	message.log_level_ = "debug";
	if(can_log(message.consumer_, message.log_level_, message.file_, message.function_)) {
		message.label_ = "DEBUG";
		print(message);
	}
}
void slim::common::log::error(Message message) {
	message.log_level_ = "error";
	if(can_log(message.consumer_, message.log_level_, message.file_, message.function_)) {
		message.label_ = "ERROR";
		print(message);
	}
}
void slim::common::log::info(std::string_view value) {
	std::lock_guard<std::mutex> lock(cout_mutex);
	std::cout << colors["INFO"] << value << colors["END"] << std::endl;
}
void slim::common::log::trace(Message message) {
	message.log_level_ = "trace";
	if(can_log(message.consumer_, message.log_level_, message.file_, message.function_)) {
		message.label_ = "TRACE";
		print(message);
	}
}
void slim::common::log::print(const Message& message) {
	std::lock_guard<std::mutex> lock((message.log_level_ == "error") ? cerr_mutex : cout_mutex);
	static std::ostream& print_stream = message.log_level_ == "error" ? std::cerr : std::cerr;
	print_stream << colors[message.label_] << message.label_ << "=>" << colors["END"];
	print_stream << std::setw(16) << colors["LINE"] << std::to_string(message.line_) << colors["END"];
	print_stream << message.separator_;
	print_stream << colors["FILE"] << message.file_ << colors["END"];
	print_stream << message.separator_;
	print_stream << colors["FUNCTION"] << message.function_ << colors["END"];
	print_stream << message.separator_;
	print_stream << colors["TEXT"] << message.text_ << colors["END"];
	print_stream << std::endl;
}
slim::common::log::Message::Message(std::string_view function, std::string_view text, std::string_view file,
	const int line, std::string_view consumer, std::string_view separator)
		: consumer_(consumer), function_(function), file_(file), text_(text), line_(line), separator_(separator) {
	process_id_ = getpid();
}
