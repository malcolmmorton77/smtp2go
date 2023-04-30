# Simple Mail Client using SMTP2go.com

### Malcolm Morton

### Class: Computer Networking

#### Info

1. create an account at [SMTP2go.com](https://www.smtp2go.com) using your email address
2. Go to Settings on the left panel > SMTP Users
3. click "Add SMTP user" button (further right)
4. choose a username and password and click "Add SMTP User"
5. you should now see your username on the SMTP users page
6. for base64 encoding of username and password, visit [base64encode.org](https://www.base64encode.org/)

#### Compile:

- `make`

#### Run:

- `./emclient`
- When prompted for "SMTP Server:" enter `mail.smtp2go.com`
- When prompted for "Port:" enter `2525`
- When prompted for "Username:" enter your username in base64 encoding
- When prompted for "Password:" enter your password in base64 encoding

#### Clean up code:
- `make clean` deletes the object file and executable file
