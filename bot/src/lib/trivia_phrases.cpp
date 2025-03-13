#include "class/TriviaGame.hpp"

TriviaGame::phrases_t TriviaGame::not_enough_players_warnings;
TriviaGame::phrases_t TriviaGame::greetings_part1;
std::string TriviaGame::greetings_part2;
std::string TriviaGame::greetings_part3;
std::string TriviaGame::greetings_part4;
TriviaGame::phrases_t TriviaGame::time_warnings;
TriviaGame::phrases_t TriviaGame::times_up_warnings;
TriviaGame::phrases_t TriviaGame::question_prompts;
TriviaGame::phrases_t TriviaGame::room_warnings;
TriviaGame::phrases_t TriviaGame::farewells;

void TriviaGame::initialize_phrases()
{
	// Not Enough Players in Channel
	TriviaGame::not_enough_players_warnings.push_back("Hmm, looks like we need one more kitty fan to start the game! Anyone want to invite their friends? 🐱");
	TriviaGame::not_enough_players_warnings.push_back("We need one more players to make this trivia party purr-fect! Spread the word! 📣");
	TriviaGame::not_enough_players_warnings.push_back("The trivia spotlight is warming up, but we need one more contestant to join before we can begin! 🎭");

	// Greetings
	TriviaGame::greetings_part1.push_back("✶⋆.˚ Hello there, friends! 🎀 Welcome to the Hello Kitty Trivia Game on the KittIRC server! ˚.⋆✶");
	TriviaGame::greetings_part1.push_back("✶⋆.˚ Greetings, everyone! 🎀 Time for Hello Kitty Trivia on KittIRC! ˚.⋆✶");
	TriviaGame::greetings_part1.push_back("✶⋆.˚ Hey kitty fans! 🎀 The Hello Kitty Trivia Game is now starting on KittIRC! ˚.⋆✶");
	TriviaGame::greetings_part1.push_back("✶⋆.˚ Meow-velous to see you all! 🎀 Welcome to Hello Kitty Trivia on KittIRC! ˚.⋆✶");
	TriviaGame::greetings_part1.push_back("✶⋆.˚ Paws what you're doing! 🎀 Hello Kitty Trivia is beginning on KittIRC! ˚.⋆✶");
	TriviaGame::greetings_part1.push_back("✶⋆.˚ Purr-fect timing, everyone! 🎀 The Hello Kitty Trivia Game is starting on KittIRC! ˚.⋆✶");
	TriviaGame::greetings_part1.push_back("✶⋆.˚ Hello wonderful friends! 🎀 Ready for some Hello Kitty Trivia on KittIRC? ˚.⋆✶");
	TriviaGame::greetings_part1.push_back("✶⋆.˚ Ribbon-tastic hello to all! 🎀 It's Hello Kitty Trivia time on KittIRC! ˚.⋆✶");
	TriviaGame::greetings_part2 = "Here's how it works: every round, I'll ask a question with " + format("multiple-choice answers", ITALIC) + ".\nYou have 30 seconds to pick the right one—" + format("just send the letter of your choice", BOLD) + " (A, B, C, or D). Easy, right? But be quick! Only your first answer will count. No take-backsies! ⏳🐱";
	TriviaGame::greetings_part3 = "When time's up, I'll reveal the correct answer and either shower you with imaginary confetti or silently judge your choices (just kidding... maybe).";
	TriviaGame::greetings_part4 = "Got it? Ready? " + format("Drop a message in the channel to confirm!", BOLD);

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
	TriviaGame::room_warnings.push_back("Game over! Someone left, and now there aren't enough players. 🚪❌");  
	TriviaGame::room_warnings.push_back("Welp, not enough players left... Game's ending! 🎬💀");  
	TriviaGame::room_warnings.push_back("A player vanished! That's game over, folks. 🎭🚶‍♂️");

	// Farewells
	TriviaGame::farewells.push_back("👏 Great game, everyone! Whether you won or just had fun, you're all champions! 🏆");  
	TriviaGame::farewells.push_back("🎉 That's a wrap! Thanks for playing—hope you had a blast! 🚀");  
	TriviaGame::farewells.push_back("🙌 Well played! Trivia is more fun with awesome players like you! 🎯");  
	TriviaGame::farewells.push_back("🏅 Game over, but the fun never ends! See you next time! 😸");  
	TriviaGame::farewells.push_back("👏 Thanks for playing! Win or lose, you made it exciting! 🎊");  

	// Teasers Before Results
	TriviaGame::teasers_before_results.push_back("Now, let's see who got it right ..."); 
	TriviaGame::teasers_before_results.push_back("Alright, time to see who nailed it!");  
	TriviaGame::teasers_before_results.push_back("Let's check who got it right!");  
	TriviaGame::teasers_before_results.push_back("Moment of truth... Who got it?");   
	TriviaGame::teasers_before_results.push_back("Drum roll, please... 🥁 Let's see who’s correct!");
}
