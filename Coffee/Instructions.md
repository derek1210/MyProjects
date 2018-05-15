# Coffee Gif Browser

## Getting Started

The application is live [here](https://mysterious-crag-44463.herokuapp.com/#/)

If you want to run the application locally:

```
$ git clone https://github.com/csc309-18s/assignment-3-coffee.git
$ cd assignment-3-coffee
$ npm install
$ npm start
```
Your browser should now open the URL `localhost:3000`; where our app lives. A window may pop up asking you to allow incoming connections.
Please click 'yes'.

## Source Code

Source code for all views is located in [client/src/views](/client/src/views).
Source code for the header and footer is located in [client/src/App.js](/client/src/App.js). The `content` div in `App.js` is
where views are swapped into based on user input. CSS style for all views is located in [/client/src/style](/client/src/style).

Server side code is located in [/server/server.js](/server/server.js). API endpoints are
defined in [docs/api.md](/docs/api.md)

## Authentication

Logins are facilitated by a `POST` request to the the `api/login` endpoint (see [docs/api.md](/docs/api.md)).
Passwords are hashed via `bcrypt` and checked against the password in the database.
Upon a successful login, two items are saved in localStorage. `isLoggedIn` is a boolean
which denotes whether or not a user is logged in. `uid` is a string which contains a logged in user's
unique identifier, which is used for facilitating saving and profile requests. 

## Using Coffee

See [Solutions.md](/Solutions.md)
