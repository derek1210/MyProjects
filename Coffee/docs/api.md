# API Endpoints

Host:
- https://mysterious-crag-44463.herokuapp.com (if running the production build)
- `localhost:8080` (if running locally)  

## api/users

Methods:
### GET:
  - Path: `/api/users`  
  - Description: Shows all users of the application
  - Return values:
    - On success, returns `200 OK` and user data (no passwords) in json form
    - On failure, returns `500`

### GET:  
  - Path: `api/users/:id`  
  - Description: Return the email and name of a user   
  - Parameters:  
    - `id`: A user's unique identifier
  - Return values:
    - On success, returns `200 OK` and user data (no passwords) in json form, for one user
    - On failure, returns specific error code

### DELETE:  
  - Path: `api/users/:id`  
  - Description: Deletes a user given by id   
  - Parameters:  
    - `id`: A user's unique identifier
  - Return values:
    - On success, returns `200 OK`
    - On failure, returns specific error code.

### PUT:
  - Path: `/api/users/:id/:variable`
  - Description: Update a given user's password, name, or email  
  - Parameters:  
    - `id`: A user's unique identifier
    - `variable`: One of: password, name, or email
  - Required Data:
    - `updater`: New value of password, name, or email
  - Return values:
    - On success, returns `200 OK`
    - On failure, returns specific error code.

## /api/register
Methods:
### POST
  - Path: `/api/register`
  - Description: Registers a new user
  - Required Data:
    - `name`: User's name
    - `email`: User's email
    - `password`: User's password
- Return values:
  - On success, returns `200 OK` and users' unique `id` for local storage
  - On failure, `500` for server error


## /api/login
Methods

### POST
  - Path: `api/login`
  - Description: Authenticate a user
  - Required Data:
    - `email`: User's email
    - `password`: User's password
  - Return values:
    - On success, returns `200 OK` and users' unique `id` for local storage
    - On failure, returns `401` for bad email or password, and `500` for server error

## /api/gifs
Methods:

### GET
  - Path: `/api/gifs/:id`
  - Description: Show gifs saved by a given user
  - Parameters:  
    - `id`: A user's unique identifier
  - Return values:
    - On success, returns `200 OK` and json containing the array of saved gifs for the given user, and the user's id
    - On failure, returns a specific error code

### PUT
  - Path: `/api/gifs/:id`
  - Description: Add a new gif to a user's private collection
  - Parameters:  
    - `id`: A user's unique identifier
  - Required Data:
    - `gif`: ID of new gif to add
  - Return values:
    - On success, returns `200 OK`
    - On failure, returns a specific error code

### DELETE
  - Path: `/api/gifs/:id`
  - Description: Remove a gif from a user's private collection
  - Parameters:  
    - `id`: A user's unique identifier
  - Required Data:
    - `gif`: ID of gif to remove
  - Return values:
    - On success, returns `200 OK`
    - On failure, returns a specific error code
