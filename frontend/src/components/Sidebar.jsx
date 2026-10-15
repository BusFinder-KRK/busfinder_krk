// TODO: Build this
import { useState } from "react"
import ParameterInput from "./minor/ParameterInput";

const PARAMETERS = [
]

export default function Sidebar() {
    const [parameterInputs, setParameterInputs] = useState([]);
    setParameterInputs(PARAMETERS.map((param) =>
        (<ParameterInput name={param.name} id={param.id} type={param.type} />)
    ))
    return (
        <div className="sidebar">
            <form name="parameters" className="sidebar-form">
                {parameterInputs}
            </form>
        </div>
    )
}