const express = require('express')
const route = express.Router()
const iotController = require('../app/controllers/IOTController')


route.get('/',iotController.index)

module.exports = route