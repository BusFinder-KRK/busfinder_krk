import Main from "./components/layout/Main"
import Header from "./components/layout/Header"
import Footer from "./components/layout/Footer"
import { useState } from "react"
import InfoWindow from "./components/InfoWindow"

export default function App() {
    const [isInfoOpen, setIsInfoOpen] = useState(false)

    return (
        <div className="everything">
            <Header onOpenInfo={() => setIsInfoOpen(true)} />
            <Main />
            <Footer />
            <InfoWindow isOpen={isInfoOpen} onClose={() => { setIsInfoOpen(false) }} />
        </div>
    )
}