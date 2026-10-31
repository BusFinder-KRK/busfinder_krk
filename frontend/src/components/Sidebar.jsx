import { useState } from "react"

import ParameterInput from "./minor/ParameterInput";

import "../style/components/Sidebar.css"

const PARAMETERS = [
    { id: "searchWindow", name: "Search window", type: "number", desc: "Maximum time for a bus/tram change. Increasing the value increases accuracy but slows down the algorithm.", defVal: 30 },
    { id: "timeForChange", name: "Punishment for change", type: "number", desc: "How much the algorithm is discouraged from picking a bus/tram change. Increasing the value makes the result include less changes on average but reduces accuracy.", defVal: 0.5 },
    { id: "walkingMultiplier", name: "Punishment for walking", type: "number", desc: "How much the algorithm is discouraged to pick walking routes. Increasing the value makes the result include less walking on average but reduces accuracy.", defVal: 1.01 },
    { id: "adjacentStops", name: "Adjacent stops", type: "number", desc: "The number of closest stops which are considered adjacent to the current stop. More increases accuracy but slows down the algorithm.", defVal: 40 },
]

export default function Sidebar() {
    const [parameterInputs, setParameterInputs] = useState(PARAMETERS);

    return (
        <div>
            <h2>
                Parameters
            </h2>
            <p>
                Parameters which give the ability to influence the result of the algorithm. The term "accuracy" used below signifies how close the result will be to the fastest solution.
            </p>
            <form name="parameters" className="sidebar-form" >
                {
                    parameterInputs.map((param) =>
                        (<ParameterInput key={param.id} name={param.name} id={param.id} type={param.type} desc={param.desc} defVal={param.defVal} />))
                }
            </form>
        </div>
    )
}