import "../style/components/Info.css"


export default function InfoWindow({ isOpen, onClose }) {
    // TODO: add project description
    if (!isOpen) return null;

    return (
        <div className="info-window">
            <button className="exit-button" onClick={onClose} aria-label="Close" />
            <h2>About this project</h2>
            <p></p>
        </div>
    );
}