import { forwardRef, useEffect, useState } from "react";
import DatePicker from "react-datepicker";

import StopInput from "./minor/StopInput"

import "../style/components/Form.css"
import "../style/components/minor/DatePicker.css"
import "react-datepicker/dist/react-datepicker.css"

function Form({ startInput, endInput, setStartInput, setEndInput, handleSubmit, setDatetime, datetime }) {

    function setNow(e) {
        e.preventDefault();
        setDatetime(new Date());
    }

    const CustomTimeInput = ({ value, onChange }) => {
        const [timeStr, setTimeStr] = useState(value || "");
        useEffect(() => {
            setTimeStr(value || "");
        }, [value]);

        const handleChange = (e) => {
            const val = e.target.value;
            setTimeStr(val);
            if (/^([01]?[0-9]|2[0-3]):[0-5][0-9]$/.test(val)) {
                onChange(val);
            }
        };

        return (
            <input
                type="text"
                value={timeStr}
                maxLength={5}
                onChange={handleChange}
            />
        )
    };


    return (
        <form name="route-form" onSubmit={handleSubmit} className="route-form">
            <StopInput inputType="start" input={startInput} setInput={setStartInput} />
            <StopInput inputType="end" input={endInput} setInput={setEndInput} />
            <div className="time-input-wrapper">
                <DatePicker
                    className="datepicker-custom fira-sans-regular"
                    selected={datetime}
                    onChange={(time) => setDatetime(time)}
                    dateFormat="dd-MM-yyyy HH:mm"

                    showTimeSelect
                    showTimeInput
                    customTimeInput={<CustomTimeInput />}

                    timeIntervals={15}
                    timeFormat="HH:mm"
                    timeCaption="Time"
                >
                </DatePicker>
                <button className="now-button" onClick={setNow}>Now</button>
            </div>
            <button className="route-button">Find route</button>
        </form >
    );
}

export default Form;