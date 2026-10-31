import ParametersContext from "../../context/ParametersContext";

import { useContext, useState } from "react";

import "../../style/components/minor/ParameterInput.css"

export default function ParameterInput({ id, name, type, desc, defVal }) {
    const { parameters, setParameters } = useContext(ParametersContext);

    const currentVal = parameters[id] ?? defVal;
    function increment(e) {
        e.preventDefault();
        const num = Number(currentVal);

        var step = 1;
        if (id === "walkingMultiplier")
            step = 0.01;
        if (id === "timeForChange")
            step = 0.1;
        const newValue = Number((num + step).toFixed(2));
        setParameter(newValue);
    }

    function decrement(e) {
        e.preventDefault();
        const num = Number(currentVal);
        if (num <= 0) return;

        var step = 1;
        if (id === "walkingMultiplier")
            step = 0.01;
        if (id === "timeForChange")
            step = 0.1;

        const newValue = Math.max(0, Number((num - step).toFixed(2)));
        setParameter(newValue);
    }

    function handleInput(e) {
        const val = e.target.value;
        const num = Number(val);

        if (val === "") {
            setParameter("");
            return;
        }

        if (!isNaN(num)) {
            const newValue = Math.max(0, num);
            setParameter(newValue);
        }
    }

    function setParameter(newValue) {
        setParameters((prevParameters) => {
            const updated = {
                ...prevParameters,
                [id]: newValue,
            };
            console.log(updated);
            return updated;
        })
    }

    return (
        <div className="parameter-input" key={id}>
            <label htmlFor={id}>
                <span className="parameter-name">
                    {name}
                </span>
                {' — '}
                <span className="parameter-desc">
                    {desc}
                </span>
            </label>
            <div className="parameter-input-field">
                <input id={id} name={name} type={type} min="0" value={currentVal} onChange={handleInput} />
                {type == "number" &&
                    <div className="custom-buttons">
                        <button type="button" onClick={() => {
                            setParameter(defVal);
                        }} className="reset">Reset</button>
                        <button type="button" onClick={increment} className="arrow">▲</button>
                        <button type="button" onClick={decrement} className="arrow">▼</button>
                    </div>
                }
            </div>
        </div>
    )
}