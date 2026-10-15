// TODO: Build this

export default function ParameterInput({ id, name, type }) {
    <div className="parameter-input">
        <label for={id}>{name}</label>
        <input id={id} name={name} type={type} key={id} />
    </div>
}