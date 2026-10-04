import { useState } from "react"
import StopInput from "./minor/StopInput"
import "../style/components/Form.css"

function Form({ startInput, endInput, setStartInput, setEndInput, handleSubmit }) { 
    

    return (
        <form name="route-form" onSubmit={handleSubmit} className="route-form">
            <StopInput inputType="start" input={startInput} setInput={setStartInput} />
            <StopInput inputType="end" input={endInput} setInput={setEndInput} />
            <button >Find route</button>
        </form>
    );
}

export default Form;