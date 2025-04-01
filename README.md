<div align="center">
  <h1>ft_irc</h1>
  <p>Create your own IRC server in C++, fully compatible with an official client.</p>
</div>

## About the project
ft_irc is a project that challenges students to create an **Internet Relay Chat (IRC)** server from scratch in **C++98**. The server must implement the core functionality of the IRC protocol as defined in **[RFC 1459](https://www.rfc-editor.org/rfc/rfc1459.html)** and be compatible with official IRC clients. This project explores network programming, socket management, and concurrent client handling.

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
- Bot functionality

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

**Mandatory:**
- **INVITE**: Invites a user to join a channel. Usage: `/INVITE [nickname] [#channel]`
- **JOIN**: Joins a channel. Usage: `/JOIN [#channel]`
- **KICK**: Kicks a user from a channel. Usage: `/KICK [#channel] [nickname] (reason)`
- **MODE**: Sets channel modes. [More details](#modes)
- **NICK**: Changes the user's nickname. Usage: `/NICK [new_nickname]`
- **PASS**: Sets a password for server access. Usage: `/PASS [password]`
- **PRIVMSG**: Sends a private message to a user or channel. Usage: `/PRIVMSG [#channel|nickname] :[message]`
- **TOPIC**: Sets the channel topic. Usage: `/TOPIC [#channel] (new_topic)`
- **USER**: Sets the user's username and realname. Usage: `/USER [username] 0 * :[realname]`

**Additional:**
- **LIST**: Lists channels and their topics. Usage: `/LIST (#channel_mask)`
- **MOTD**: Displays the server's message of the day. Usage: `/MOTD`
- **NAMES**: Lists users in a channel. Usage: `/NAMES (#channel)`
- **PART**: Leaves a channel. Usage: `/PART [#channel] (reason)`
- **PING**: Pings the server. Usage: `/PING [message]`
- **QUIT**: Disconnects from the server. Usage: `/QUIT (reason)`
- **WHO**: Lists users in the server. Usage: `/WHO (#channel|user_mask)`

### Modes
- **i (Invite-only)**: When set, users can only join the channel if they are explicitly invited by a channel operator. This restricts channel access to invited users only. Usage:
  - `/MODE [#channel] +i`
  - `/MODE [#channel] -i`
- **t (Topic protection)**: When enabled, only channel operators can change the channel topic. This prevents regular users from modifying the topic. Usage:
  - `/MODE [#channel] +t`
  - `/MODE [#channel] -t`
- **k (Key/Password)**: Requires users to provide the correct password when joining the channel. Usage:
  - `/MODE [#channel] +k [password]`
  - `/MODE [#channel] -k`
- **o (Operator)**: Grants operator status to a user, providing them with administrative privileges in the channel. Operators can set modes, kick users, etc. Usage:
  - `/MODE [#channel] +o [nickname]`
  - `/MODE [#channel] -o [nickname]`
- **l (User limit)**: Sets a maximum number of users who can join the channel. Once this limit is reached, no more users can join until someone leaves. Usage:
  - `/MODE [#channel] +l [limit]`
  - `/MODE [#channel] -l`

To set these modes, use the MODE command with the appropriate parameters. Multiple modes can be set at once, for example: `/MODE #channel +itk password`
