
const express = require('express')
const app = express()
const port = 3000
const morgan = require('morgan')
const hbs = require('express-handlebars')

const route = require('./routes')






app.use(express())
app.use(morgan('combined'))

app.engine('.hbs',hbs.engine({
  extname:'.hbs'
})) 
app.set('view engine', '.hbs')







app.use(express.static('./src/public/'))
app.set('views', './src/views')


app.listen(port, () => {
    console.log(`Đã khởi chạy Website của bạn ở  http://localhost:${port}`)
  })
  
route(app)