#include "class/TriviaGame.hpp"

std::string				TriviaGame::greetings_header;
TriviaGame::phrases_t	TriviaGame::greetings_subheader;
std::string				TriviaGame::game_rules;
std::string				TriviaGame::ask_ready;

TriviaGame::phrases_t	TriviaGame::not_enough_players_warnings;
TriviaGame::phrases_t	TriviaGame::time_warnings;
TriviaGame::phrases_t	TriviaGame::times_up_warnings;
TriviaGame::phrases_t	TriviaGame::question_prompts;
std::string				TriviaGame::early_leaving_warning_part1;
std::string				TriviaGame::early_leaving_warning_part2;
TriviaGame::phrases_t	TriviaGame::farewells;

void TriviaGame::initialize_phrases()
{
	// Not Enough Players in Channel
	TriviaGame::not_enough_players_warnings.push_back("Hmm, looks like we need one more kitty fan to start the game! Anyone want to invite their friends? 🐱");
	TriviaGame::not_enough_players_warnings.push_back("We need one more players to make this trivia party purr-fect! Spread the word! 📣");
	TriviaGame::not_enough_players_warnings.push_back("The trivia spotlight is warming up, but we need one more contestant to join before we can begin! 🎭");

	// Greetings
	TriviaGame::greetings_header = "🎀 Welcome to " + format("Hello Kitty Trivia", COL_PINK) + " on KittIRC! 🎀";
	TriviaGame::greetings_subheader.push_back("🩷 Test your knowledge in the cutest trivia game ever! 🩷");
	TriviaGame::greetings_subheader.push_back("🐱 Think you know everything? Let's find out! 🐱");
	TriviaGame::greetings_subheader.push_back("🌟 Get ready for a fun and purr-fectly adorable trivia game! 🌟");
	TriviaGame::greetings_subheader.push_back("🎠 Get your bows ready—it's time for some trivia! 🎠");
	TriviaGame::greetings_subheader.push_back("🌸 A trivia game as sweet as a candy. Are you ready? 🌸");
	TriviaGame::greetings_subheader.push_back("🏆 Only the biggest Hello Kitty fans will triumph—are you one of them? 🏆");

	TriviaGame::game_rules = format("📖 HOW TO PLAY:\n", COL_CYAN);
	TriviaGame::game_rules += "1️⃣ Each round, I'll ask a multiple-choice question.\n";
	TriviaGame::game_rules += "2️⃣ You have " + format("30 seconds", BOLD) + "to answer—just send the " + format("letter", BOLD) + "of your choice (A, B, C, or D).\n";
	TriviaGame::game_rules += "3️⃣ Only your " + format("first answer", BOLD) + " counts! No take-backsies! ⏳🐱\n\n";

	TriviaGame::game_rules += format("🎉 WHAT HAPPENS NEXT?\n", COL_CYAN);
	TriviaGame::game_rules += "Once time's up, I'll reveal the correct answer and:\n";
	TriviaGame::game_rules += "✨ Shower you with imaginary confetti if you're " + format("right", ITALIC) + "!\n";
	TriviaGame::game_rules += "🤨 Silently judge your choices if you're " + format("wrong", ITALIC) + " (just kidding... maybe).\n\n";

	TriviaGame::ask_ready = "💬 Got it? Ready? " + format("Drop a message in the channel to confirm!", BOLD);

	// Time Warnings
	TriviaGame::time_warnings.push_back("5 sec left! Panic! ⏳");
	TriviaGame::time_warnings.push_back("Tick-tock! Brain in turbo! 🏎️💨");
	TriviaGame::time_warnings.push_back("5 sec! Choose wisely or regret! 😼");
	TriviaGame::time_warnings.push_back("Still thinking? THINK FASTER! ⚡");
	TriviaGame::time_warnings.push_back("HELLO?? Time's up soon! 🏃‍♀️💨");
	TriviaGame::time_warnings.push_back("5 sec! Believe in miracles? 😹");
	TriviaGame::time_warnings.push_back("Quick! Eeny, meeny… PICK! 🎲");
	TriviaGame::time_warnings.push_back("No answer? Time to scream! 🔥");
	TriviaGame::time_warnings.push_back("Almost out! Choose or be judged! 👀");
	TriviaGame::time_warnings.push_back("5 sec! Trust your gut? 🤔");

	// Time's up Warnings
	TriviaGame::times_up_warnings.push_back("Times up!");
	TriviaGame::times_up_warnings.push_back("Pencils down, kitty fans!");
	TriviaGame::times_up_warnings.push_back("Ding ding ding! Time's over!");
	TriviaGame::times_up_warnings.push_back("That's a wrap on the clock! ⏰");
	TriviaGame::times_up_warnings.push_back("The buzzer has buzzed! Time's up!");
	TriviaGame::times_up_warnings.push_back("The clock says no more guessing!");
	TriviaGame::times_up_warnings.push_back("Time to stop! The clock has spoken!");
	TriviaGame::times_up_warnings.push_back("And we're out of time!");
	TriviaGame::times_up_warnings.push_back("Tick-tock-STOP! Time's up!");
	TriviaGame::times_up_warnings.push_back("Paws off the keyboard! Time's up!");

	// Question Prompts
	TriviaGame::question_prompts.push_back("Lock in your answer! 🏹");
	TriviaGame::question_prompts.push_back("Take your best shot! 🎯");
	TriviaGame::question_prompts.push_back("Think fast! What's your pick? ⏳");
	TriviaGame::question_prompts.push_back("Make your choice, hero! 🦸");
	TriviaGame::question_prompts.push_back("Time's ticking—what's your answer? ⏰");
	TriviaGame::question_prompts.push_back("Go with your gut! What'll it be? 🤔");
	TriviaGame::question_prompts.push_back("Choose wisely! 🧠✨");
	TriviaGame::question_prompts.push_back("What's your final answer? 🔥");
	TriviaGame::question_prompts.push_back("Trust your instincts—what do you say? 🧐");
	TriviaGame::question_prompts.push_back("Don't overthink it! Pick one! 🚀");

	// A player left -> not enough players left
	TriviaGame::early_leaving_warning_part1 = "Why'd you leave? Now it's just me, ";
	TriviaGame::early_leaving_warning_part2 = ", and a dead game 🎮💀";

	// Farewells
	TriviaGame::farewells.push_back("👏 Great game, everyone! Whether you won or just had fun, you're all champions! 🏆");  
	TriviaGame::farewells.push_back("🎉 That's a wrap! Thanks for playing—hope you had a blast! 🚀");  
	TriviaGame::farewells.push_back("🙌 Well played! Trivia is more fun with awesome players like you! 🎯");  
	TriviaGame::farewells.push_back("🏅 Game over, but the fun never ends! See you next time! 😸");  
	TriviaGame::farewells.push_back("👏 Thanks for playing! Win or lose, you made it exciting! 🎊");  
}
