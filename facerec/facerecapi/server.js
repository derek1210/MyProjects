const express = require('express');
const app = express();
const bodyParser = require('body-parser');
const bcrypt = require('bcrypt');
const saltRounds = 10;
const cors = require('cors');
const db = require('knex')({
    client: 'pg',
    connection: {
      connectionString: process.env.DATABASE_URL,
      ssl: true
    }
});

app.use(bodyParser.json());
app.use(cors());

app.get('/', (req,res) => {
    res.send(database.users);
});

// /signin -> POST = sucess/fail
app.post('/signin', (req, res) => {
    db.select('email', 'hash').from('login')
    .where('email', '=', req.body.email)
    .then(data => {
        const isValid = bcrypt.compareSync(req.body.pw, data[0].hash);
        if (isValid) {
            return db.select('*')
            .from('users')
            .where('email', '=', req.body.email)
            .then(user => res.json(user[0]))
            .catch(err => res.status(400).json('Unable to get user'))
        } else {
            res.status(400).json('Wrong email or password');
   []     }
    }).catch(err => res.status(400).json('Wrong email or password'))
});

// /register -> POST = user 
app.post('/register', (req, res) => {
    const {email, pw, name} = req.body;
    const hash = bcrypt.hashSync(pw, saltRounds);
    db.transaction(trx => {
        trx.insert({
            hash: hash,
            email: email
        }).into('login')
        .returning('email')
        .then(loginEmail => {
            return trx('users')
                .returning('*')
                .insert({
                    email: loginEmail[0],
                    name: name,
                    joined: new Date()
                }).then(user => {
                    res.json(user[0]);
                })
        })
        .then(trx.commit)
        .catch(trx.rollback)
    })
    .catch(err => res.status(400).json("Unable to register"));
});


// /profile/:userId -> GET = user
app.get('/profile/:id', (req, res) => {
    const {id} = req.params;
    db.select('*').from('users').where({id: id}).then(user => {
        if (user.length) {
            res.json(user[0]);
        } else {
            res.status(400).json('Not Found');
        }
    }).catch(err => res.status(400).json('Error getting user'))
});

// /image -> PUT = user 
app.put('/image', (req, res) => {
    const {id} = req.body;
    db('users').where('id', '=', id)
    .increment('entries', 1)
    .returning('entries')
    .then(entries => {
        res.json(entries[0]);
    }).catch(err => res.status(400).json('Unable to get entries'));
});

app.listen(process.env.PORT || 3000, () => {
    console.log(`App is running on PORT ${process.env.PORT}`);
});