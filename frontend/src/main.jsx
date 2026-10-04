import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './style/index.css'
import App from './App.jsx'
import { StopsProvider } from './context/StopsContext.jsx'

createRoot(document.getElementById('root')).render(
  <StrictMode>
    <StopsProvider>
      <App />
    </StopsProvider>
  </StrictMode>,
)
