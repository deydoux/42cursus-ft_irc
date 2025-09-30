<div align="center">
  <img height="64" src="https://github.com/user-attachments/assets/87e625e6-84f7-4567-9a02-cf4def9dd5c7">
  <h1>ft_irc</h1>
  <p>Create your own IRC server in C++, fully compatible with an official client.</p>
</div>

## About the project
ft_irc is a project that challenges students to create an **Internet Relay Chat (IRC)** server from scratch in **C++98**. The server must implement the core functionality of the IRC protocol as defined in **[RFC 1459](https://www.rfc-editor.org/rfc/rfc1459.html)** and be compatible with official IRC clients. This project explores network programming, socket management, and concurrent client handling.

![Screenshots](https://github.com/user-attachments/assets/f7003ecf-1c86-4093-b0e5-14ad8f2538bc)

### Key Requirements
- Implementation in C++98 standard
- TCP/IP (IPv4) communication
- Non-blocking I/O operations with `poll()`
- Support for multiple simultaneous clients
- Channel and user management
- Proper error handling and graceful disconnection
- Implementation of essential [IRC commands](#commands)
- Support for [channel modes](#modes)

### Bonus features
- Additional IRC features beyond the basic requirements
- Improved error handling and logging
- File transfer capabilities (DCC)
- AI chatbot integration
- JSON parser

### Skills Learned
- Network programming with sockets
- Concurrent programming and I/O multiplexing
- Protocol implementation
- Client-server model
- Resource management in C++
- Error handling in distributed systems
- Parsing and command processing

## Implementation
### Commands
Our IRC server implements a comprehensive set of commands that enable users to connect, communicate, and manage channels effectively. These commands are divided into mandatory commands (required by the IRC protocol) and additional commands that enhance usability. All commands follow standard IRC syntax and are compatible with official IRC clients. Each command serves a specific purpose in user authentication, channel management, or communication functionality.

The server processes these commands efficiently with proper error handling and provides appropriate responses according to the IRC protocol specifications. Below is a detailed list of all available commands with their usage syntax:

<details open>
  <summary><strong>Mandatory commands</strong></summary>

- <strong>INVITE</strong>: Invites a user to join a channel. <em><code>/INVITE [nickname] [#channel]</code></em>
- <strong>JOIN</strong>: Joins a channel. <em><code>/JOIN [#channel]</code></em>
- <strong>KICK</strong>: Kicks a user from a channel. <em><code>/KICK [#channel] [nickname] (reason)</code></em>
- <strong>MODE</strong>: Sets channel modes. <a href="#modes">More details</a>
- <strong>NICK</strong>: Changes the user's nickname. <em><code>/NICK [new_nickname]</code></em>
- <strong>PASS</strong>: Sets a password for server access. <em><code>/PASS [password]</code></em>
- <strong>PRIVMSG</strong>: Sends a private message to a user or channel. <em><code>/PRIVMSG [#channel|nickname] :[message]</code></em>
- <strong>TOPIC</strong>: Sets the channel topic. <em><code>/TOPIC [#channel] (new_topic)</code></em>
- <strong>USER</strong>: Sets the user's username and realname. <em><code>/USER [username] 0 * :[realname]</code></em>
</details>

<details open>
  <summary><strong>Additional commands</strong></summary>

- <strong>LIST</strong>: Lists channels and their topics. <em><code>/LIST (#channel_mask)</code></em>
- <strong>MOTD</strong>: Displays the server's message of the day. <em><code>/MOTD</code></em>
- <strong>NAMES</strong>: Lists users in a channel. <em><code>/NAMES (#channel)</code></em>
- <strong>PART</strong>: Leaves a channel. <em><code>/PART [#channel] (reason)</code></em>
- <strong>PING</strong>: Pings the server. <em><code>/PING [message]</code></em>
- <strong>QUIT</strong>: Disconnects from the server. <em><code>/QUIT (reason)</code></em>
- <strong>WHO</strong>: Lists users in the server. <em><code>/WHO (#channel|user_mask)</code></em>
</details>

### Modes
- **i (Invite-only)**: When set, users can only join the channel if they are explicitly invited by a channel operator. This restricts channel access to invited users only.
  - *`/MODE [#channel] +i`*
  - *`/MODE [#channel] -i`*
- **t (Topic protection)**: When enabled, only channel operators can change the channel topic. This prevents regular users from modifying the topic.
  - *`/MODE [#channel] +t`*
  - *`/MODE [#channel] -t`*
- **k (Key/Password)**: Requires users to provide the correct password when joining the channel.
  - *`/MODE [#channel] +k [password]`*
  - *`/MODE [#channel] -k`*
- **o (Operator)**: Grants operator status to a user, providing them with administrative privileges in the channel. Operators can set modes, kick users, etc.
  - *`/MODE [#channel] +o [nickname]`*
  - *`/MODE [#channel] -o [nickname]`*
- **l (User limit)**: Sets a maximum number of users who can join the channel. Once this limit is reached, no more users can join until someone leaves.
  - *`/MODE [#channel] +l [limit]`*
  - *`/MODE [#channel] -l`*

To set these modes, use the **MODE** command with the appropriate parameters. Multiple modes can be set at once, for example: `/MODE #channel +itk password`

## References
<img align="right" src="https://github.com/user-attachments/assets/5dbfe00e-5a88-4a49-8932-70b8ba45ec15">

- [Socket Programming in C](https://geeksforgeeks.org/c/socket-programming-cc/)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)

### RFCs
- [RFC 1459 "Internet Relay Chat Protocol"](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2810 "Internet Relay Chat: Architecture"](https://datatracker.ietf.org/doc/html/rfc2810)
- [RFC 2811 "Internet Relay Chat: Channel Management"](https://datatracker.ietf.org/doc/html/rfc2811)
- [RFC 2812 "Internet Relay Chat: Client Protocol"](https://datatracker.ietf.org/doc/html/rfc2812)

