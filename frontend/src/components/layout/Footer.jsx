import "../../style/components/layout/Footer.css";

export default function Footer() {
    const currYear = new Date().getFullYear();
    return (
        <div className="ftr">
            <p>
                &copy; {currYear} Emilia Czopor, Witold Waligóra. All rights reserved.
            </p>
        </div>
    );
}